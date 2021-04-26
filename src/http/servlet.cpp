#include "servlet.h"
#include "process.h"
#include "fiber.h"
#include "worker.h"
#include "server.h"
#include "config.h"
#include "module.h"
#include "log.h"
#include "authorization.h"


namespace bifang
{

namespace http
{

SystemLogger();

NotFoundServlet::NotFoundServlet()
    :Servlet("NotFoundServlet")
{
}

int32_t NotFoundServlet::handle(HttpRequest::ptr request,
                             HttpResponse::ptr response,
                             HttpSession::ptr session)
{
    response->setStatus(HttpStatus::NOT_FOUND);
    return 0;
}


ServletDispatch::ServletDispatch()
    :Servlet("ServletDispatch")
{
    m_default.reset(new NotFoundServlet);
}

int32_t ServletDispatch::handle(HttpRequest::ptr request,
                             HttpResponse::ptr response,
                             HttpSession::ptr session)
{
    auto slt = getMatchedServlet(request->getPath());
    if (slt)
    {
        Authorization::ptr auth = AuthorizationMgr::GetInstance()->get(slt);
        if (auth && !auth->check(request, response))
            return 0;
        slt->handle(request, response, session);
    }

    return 0;
}

void ServletDispatch::addServlet(const std::string& uri, Servlet::ptr slt)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = slt;
}

void ServletDispatch::addServlet(const std::string& uri,
                          FunctionServlet::Callback cb)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = std::make_shared<FunctionServlet>(cb);
}

void ServletDispatch::addGlobServlet(const std::string& uri, Servlet::ptr slt)
{
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globs.begin(); it != m_globs.end(); it++)
    {
        if (it->first == uri)
        {
            m_globs.erase(it);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, slt));
}

void ServletDispatch::addGlobServlet(const std::string& uri,
                          FunctionServlet::Callback cb)
{
    return addGlobServlet(uri, std::make_shared<FunctionServlet>(cb));
}

void ServletDispatch::delServlet(const std::string& uri)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string& uri)
{
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globs.begin(); it != m_globs.end(); it++)
    {
        if (it->first == uri)
        {
            m_globs.erase(it);
            break;
        }
    }
}

Servlet::ptr ServletDispatch::getServlet(const std::string& uri)
{
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_datas.find(uri);
    return it == m_datas.end() ? nullptr : it->second;
}

Servlet::ptr ServletDispatch::getGlobServlet(const std::string& uri)
{
    RWMutexType::ReadLock lock(m_mutex);
    for (auto it = m_globs.begin(); it != m_globs.end(); it++)
    {
        if (fnmatch(it->first.c_str(), uri.c_str(), 0) == 0)
            return it->second;
    }
    return nullptr;
}

Servlet::ptr ServletDispatch::getMatchedServlet(const std::string& uri)
{
    auto ret = getServlet(uri);
    if (ret != nullptr)
        return ret;
    auto rret = getGlobServlet(uri);
    if (rret != nullptr)
        return rret;
    return m_default;
}

void ServletDispatch::listServletCreator(std::map<std::string,
                          Servlet::ptr>& infos)
{
    RWMutexType::ReadLock lock(m_mutex);
    for (auto& i : m_datas)
        infos[i.first] = i.second;
}

void ServletDispatch::listGlobServletCreator(std::map<std::string,
                          Servlet::ptr>& infos)
{
    RWMutexType::ReadLock lock(m_mutex);
    for (auto& i : m_globs)
        infos[i.first] = i.second;
}


ConfigServlet::ConfigServlet()
    :Servlet("ConfigServlet")
{
}

int32_t ConfigServlet::handle(HttpRequest::ptr request, 
                           HttpResponse::ptr response,
                           HttpSession::ptr session)
{
    Json::Value root;
    ConfigMgr::GetInstance()->visit(
        [&root](ConfigBase::ptr base)
        {
            Json::Value n;
            try
            {
                n = Json::Value(base->toString());
            }
            catch (...)
            {
                log_error << "config visit error!";
                return -1;
            }
            root[base->getName()] = n;
            root[base->getName() + " description"] = base->getDescription();
        });

    Json::StreamWriterBuilder writer;
    std::ostringstream os;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
    jsonWriter->write(root, &os);
    response->setConetentType("*.json");
    response->setLastModified("", nullptr);
    response->setContent(os.str());

    return 0;
}


StatusServlet::StatusServlet()
    :Servlet("StatusServlet")
{
}

int32_t StatusServlet::handle(HttpRequest::ptr request,
                           HttpResponse::ptr response,
                           HttpSession::ptr session)
{
#define HEAD(s) \
    ss << "<h3>" s ":</h3><ul>"

#define XX(s) \
    ss << "<li>" << s << "</li>"

#define TAIL() \
    ss << "</ul><hr/>"

    std::stringstream ss;
    ss << "<html><head><title>Status</title></head><body>"
          "<center><h1>" << SYS_VERSION << "</h1></center>";

    HEAD("Modules");
    std::vector<Module::ptr> modules;
    ModuleMgr::GetInstance()->getList(modules);
    for (auto& m : modules)
        XX(m->getId() + ": " + m->toString());
    TAIL();

    HEAD("Host Network");
    XX("host: " + getHostName());
    XX("ipv4: " + getIPv4());
    TAIL();

    HEAD("Servers");
    const std::map<std::string, std::vector<TcpServer::ptr> >& servers = getListOfServer();
    for (auto& it : servers)
    {
        ss << "<li>" << it.first << ":<br/>";
        for (auto& i : it.second)
        {
            std::vector<std::string> strs = StringUtil::split(i->toString(), '\n');
            for (auto& str : strs)
                ss << str << "<br/>";

            HttpServer::ptr httpserver = std::dynamic_pointer_cast<HttpServer>(i);
            if (httpserver)
            {
                auto servlet = httpserver->getServletDispatch();
                if (servlet)
                {
                    std::map<std::string, Servlet::ptr> infos;
                    servlet->listServletCreator(infos);
                    if (!infos.empty())
                    {
                        ss << "[Servlets: ";
                        int comma_flag = false;
                        for (auto& i : infos)
                        {
                            if (comma_flag)
                                ss << ", ";
                            ss << i.first << ": " << i.second->getName();
                            comma_flag = true;
                        }
                        ss << "]<br/>";
                    }
                    infos.clear();
                    servlet->listGlobServletCreator(infos);
                    if (!infos.empty())
                    {
                        ss << "[Servlets.Globs: ";
                        int comma_flag = false;
                        for (auto& i : infos)
                        {
                            if (comma_flag)
                                ss << ", ";
                            ss << i.first << ": " << i.second->getName();
                            comma_flag = true;
                        }
                        ss << "]<br/>";
                    }
                }
            }
        }

        ss << "</li>";
    }
    TAIL();

    HEAD("System");
    XX("daemon id: " + std::to_string(ProcessMgr::GetInstance()->getParentId()));
    XX("main id: " + std::to_string(ProcessMgr::GetInstance()->getMainId()));
    XX("daemon start time: " + time_to_string(ProcessMgr::GetInstance()->getParentStartTime()));
    XX("main start time: " + time_to_string(ProcessMgr::GetInstance()->getMainStartTime()));
    XX("daemon running_time: " + time_difference(time(0) - ProcessMgr::GetInstance()->getParentStartTime()));
    XX("main running_time: " + time_difference(time(0) - ProcessMgr::GetInstance()->getMainStartTime()));
    XX("restart count: " + std::to_string(ProcessMgr::GetInstance()->getRestartCount()));
    XX("fibers: " + std::to_string(Fiber::total()));
    TAIL();

    ss << "</body></html>";
    response->setConetentType("*.html");
    response->setLastModified("", nullptr);
    response->setContent(ss.str());
    return 0;

#undef HEAD
#undef XX
#undef TAIL
}

}

}
