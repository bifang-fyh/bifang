#include "bifang.h"

namespace bifang
{

SystemLogger();

static Config<std::vector<TcpServerConf> >::ptr g_servers_config =
    ConfigMgr::GetInstance()->get("servers", std::vector<TcpServerConf>(), "servers config");

class Application : public ServerManager
{
public:
    Application(const std::string& config_path)
        :ServerManager(config_path)
    {
    }

private:
    virtual int main_fiber() override;

    void startServer(std::vector<TcpServer::ptr>& servers);
};

// virtual
int Application::main_fiber()
{
    std::vector<Module::ptr> modules;
    ModuleMgr::GetInstance()->init();
    ModuleMgr::GetInstance()->getList(modules);
    bool has_error = false;
    for (auto& i : modules)
    {
        if (!i->onLoad())
        {
            log_error << i->toString();
            has_error = true;
            break;
        }
    }
    if (has_error)
        _exit(0);

    std::vector<TcpServer::ptr> servers;
    startServer(servers);

    for (auto& i : modules)
        i->onServerReady();

    for (auto& i : servers)
        i->start();

    for (auto& i : modules)
        i->onServerUp();

    return 0;
}

void Application::startServer(std::vector<TcpServer::ptr>& servers)
{
    auto servers_confs = g_servers_config->getValue();

    for (auto& i : servers_confs)
    {
        log_debug << std::endl << LexicalCast<TcpServerConf, std::string>()(i);

        std::vector<Address::ptr> addresses;
        for (auto& a : i.address)
        {
            size_t pos = a.find(":");
            if (pos == std::string::npos)
                continue;
            uint16_t port = atoi(a.substr(pos + 1).c_str()); // ¶Ë¿ÚºÅ
            auto addr = Address::create(a.substr(0, pos).c_str(), port);
            if (addr)
            {
                addresses.push_back(addr);
                continue;
            }

            std::vector<std::pair<Address::ptr, uint32_t> > result;
            if (Address::getInterface(result, a.substr(0, pos)))
            {
                for (auto& x : result)
                {
                    (x.first)->setPort(atoi(a.substr(pos + 1).c_str()));
                    addresses.push_back(x.first);
                }
                continue;
            }

            auto aaddr = Address::getAddrInfo(a);
            if (aaddr)
            {
                addresses.push_back(aaddr);
                continue;
            }
            log_error << "invalid address: " << a;
            _exit(0);
        }
        IOManager* accept_worker = IOManager::getThis();
        IOManager* process_worker = IOManager::getThis();
        if (!i.accept_worker.empty())
        {
            accept_worker = WorkerMgr::GetInstance()->getAsIOManager(i.accept_worker).get();
            if (!accept_worker)
            {
                log_error << "accept_worker: " << i.accept_worker
                    << " not exists";
                _exit(0);
            }
        }
        if (!i.process_worker.empty())
        {
            process_worker = WorkerMgr::GetInstance()->getAsIOManager(i.process_worker).get();
            if (!process_worker)
            {
                log_error << "process_worker: " << i.process_worker << " not exists";
                _exit(0);
            }
        }

        TcpServer::ptr server;
        if (i.type == "http")
        {
            server.reset(new http::HttpServer(i.keepalive,
                            accept_worker, process_worker,
                            i.gzip, i.gzip_min_length, i.gzip_comp_level));
        }
        else if (i.type == "ws")
        {
            server.reset(new ws::WSServer(accept_worker, process_worker));
        }
        else
        {
            log_error << "invalid server type=" << i.type << std::endl
                << LexicalCast<TcpServerConf, std::string>()(i);
            _exit(0);
        }

        server->setRecvTimeout(i.timeout);

        if (!i.name.empty())
            server->setName(i.name);

        std::vector<Address::ptr> fails;
        if (!server->bind(addresses, fails, i.ssl))
        {
            for (auto& x : fails)
                log_error << "bind address fail: " << *x;
            _exit(0);
        }
        if (i.ssl)
        {
            if (!server->loadCertificates(i.cert_file, i.key_file))
            {
                log_error << "loadCertificates fail, cert_file="
                    << i.cert_file << " key_file=" << i.key_file;
            }
        }
        g_servers[i.type].push_back(server);
        servers.push_back(server);
    }
}

}


int main(int argc, char* argv[])
{
    bifang::Application* app = new bifang::Application("./configs");
    if (app->init(argc, argv))
        return app->run();

    return 0;
}
