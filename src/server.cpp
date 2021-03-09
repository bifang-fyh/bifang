#include "bifang.h"


namespace bifang
{

SystemLogger();

static Config<std::string>::ptr g_pid_file =
    ConfigMgr::GetInstance()->get("system.pid_file", std::string("bifang.pid"), "system pid file config");

static Config<std::vector<TcpServerConf> >::ptr g_servers_config =
    ConfigMgr::GetInstance()->get("servers", std::vector<TcpServerConf>(), "servers config");

static Config<std::unordered_map<std::string, uint32_t> >::ptr g_worker_config =
       ConfigMgr::GetInstance()->get("workers", std::unordered_map<std::string, uint32_t>(), "worker config");

bool ServerManager::init(int argc, char** argv)
{
    if (m_running)
        return false;
    m_argc = argc;
    m_argv = argv;

    EnvMgr::GetInstance()->addHelp("s", "start with the terminal(default)");
    EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    EnvMgr::GetInstance()->addHelp("c", "config path default: ./configs");
    EnvMgr::GetInstance()->addHelp("p", "print help");

    if (!EnvMgr::GetInstance()->init(argc, argv)
      || EnvMgr::GetInstance()->has("p"))
    {
        EnvMgr::GetInstance()->printHelp();
        return false;
    }

    std::string conf_path = EnvMgr::GetInstance()->getConfigPath();
    ConfigMgr::GetInstance()->load(conf_path);
    std::string pidfile = EnvMgr::GetInstance()->getAbsolutePath(g_pid_file->getValue());
    if (FileUtil::CheckPidfile(pidfile))
    {
        log_error << "server is running: " << pidfile;
        return false;
    }
    {
        std::ofstream ofs(pidfile);
        if (!ofs)
        {
            log_error << "open pidfile [" << pidfile << "] failed";
            return false;
        }
        ofs << getpid();
    }

    ModuleMgr::GetInstance()->init();

    return true;
}

bool ServerManager::run()
{
    if (m_running)
        return false;
    m_running = true;
    bool is_daemon = EnvMgr::GetInstance()->has("d");
    return ProcessMgr::GetInstance()->start(m_argc, m_argv, 
               std::bind(&ServerManager::main, this, std::placeholders::_1,
               std::placeholders::_2), is_daemon);
}

void ServerManager::startServer(std::vector<TcpServer::ptr>& servers)
{
    auto servers_confs = g_servers_config->getValue();
    WorkerMgr::GetInstance()->init(g_worker_config->getValue());

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
        m_servers[i.type].push_back(server);
        servers.push_back(server);
    }
}

bool ServerManager::getServer(const std::string& type,
                      std::vector<TcpServer::ptr>& servers)
{
    auto it = m_servers.find(type);
    if (it == m_servers.end())
        return false;
    servers = it->second;
    return true;
}

std::map<std::string, std::vector<TcpServer::ptr> >& ServerManager::GetListOfServer()
{
    return m_servers;
}

// private
int ServerManager::main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);
    log_info << "main start";

    m_mainIOManager.reset(new IOManager(1, true, "main"));
    m_mainIOManager->schedule(std::bind(&ServerManager::main_fiber, this));
    m_mainIOManager->addTimer(60000,
        []()
        {
            log_debug << "How old are you? I'm fine, thank you, and you?";
        }, true);
    m_mainIOManager->stop();

    return 0;
}

// private
int ServerManager::main_fiber()
{
    std::vector<Module::ptr> modules;
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

}
