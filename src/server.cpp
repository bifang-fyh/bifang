#include "bifang.h"


namespace bifang
{

SystemLogger();

static Config<std::string>::ptr g_pid_file =
    ConfigMgr::GetInstance()->get("system.pid_file", std::string("bifang.pid"), "system pid file config");

static Config<std::unordered_map<std::string, uint32_t> >::ptr g_worker_config =
       ConfigMgr::GetInstance()->get("workers", std::unordered_map<std::string, uint32_t>(), "worker config");

static Config<std::vector<TcpServerConf> >::ptr g_servers_config =
    ConfigMgr::GetInstance()->get("servers", std::vector<TcpServerConf>(), "servers config");

static std::map<std::string, std::vector<TcpServer::ptr> > g_servers;
bool getServer(const std::string& type, std::vector<TcpServer::ptr>& servers)
{
    auto it = g_servers.find(type);
    if (it == g_servers.end())
        return false;
    servers = it->second;
    return true;
}
const std::map<std::string, std::vector<TcpServer::ptr> >& getListOfServer()
{
    return g_servers;
}

////////////////////////////////////////////////////////////////////////////////
static void reopen_handler(int signo)
{
    LoggerMgr::GetInstance()->reopen();
}

ServerManager::ServerManager(const std::string& config_path)
    :m_config_path(config_path)
{
}

bool ServerManager::init(int argc, char** argv)
{
    if (m_running)
        return false;
    m_argc = argc;
    m_argv = argv;

    EnvMgr::GetInstance()->addHelp("r", "reopen log file");
    EnvMgr::GetInstance()->addHelp("s", "stop server");
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
    if (!conf_path.empty())
        ConfigMgr::GetInstance()->load(conf_path);
    else
        ConfigMgr::GetInstance()->load(m_config_path);

    std::string pidfile = EnvMgr::GetInstance()->getAbsolutePath(g_pid_file->getValue());
    pid_t pid;
    bool is_running = FileUtil::CheckPidfile(pidfile, &pid);
    if (EnvMgr::GetInstance()->has("s"))
    {
        if (is_running)
        {
            if (!kill(pid, SIGUSR2))
                std::cout << "server stop sucessful, pid:" << pid << std::endl;
            else
                std::cout << "server stop fail, pid:" << pid << std::endl;
        }
        else
            std::cout << "server not running, can't stop" << std::endl;
        return false;
    }
    else if (EnvMgr::GetInstance()->has("r"))
    {
        if (is_running)
            if (!kill(pid, SIGUSR1))
                std::cout << "reopen log file sucessful" << std::endl;
            else
                std::cout << "reopen log file fail" << std::endl;
        else
            std::cout << "server not running, can't reopen log file" << std::endl;
        return false;
    }
    if (is_running)
    {
        std::cout << "server is running, pid:" << pid << std::endl;
        return false;
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

// private
int ServerManager::main(int argc, char** argv)
{
    {
        std::string pidfile = EnvMgr::GetInstance()->getAbsolutePath(g_pid_file->getValue());
        std::ofstream ofs(pidfile);
        if (!ofs)
        {
            std::cout << "open pidfile [" << pidfile << "] failed" << std::endl;
            return false;
        }
        ofs << getpid();
    }

    signal(SIGPIPE, SIG_IGN);
    struct sigaction sa;
    memset(&sa, 0x00, sizeof(struct sigaction));
    sa.sa_handler = reopen_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL))
    {
        std::cout << "sigaction(SIGUSR1) failed" << std::endl;
        return -1;
    }

    m_mainIOManager.reset(new IOManager(1, true, "main"));
    LoggerMgr::GetInstance()->reopen();
    WorkerMgr::GetInstance()->init(g_worker_config->getValue());
    log_info << "main start";
    m_mainIOManager->schedule(std::bind(&ServerManager::main_fiber, this));
    m_mainIOManager->addTimer(60 * 000,
        []()
        {
            log_debug << "How old are you? I'm fine, thank you, and you?";
        }, true);
    m_mainIOManager->stop();

    return 0;
}

}
