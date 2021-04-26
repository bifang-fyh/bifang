#include "bifang.h"

namespace bifang
{

SystemLogger();

class Application : public ServerManager
{
public:
    Application(const std::string& config_path)
        :ServerManager(config_path)
    {
    }

protected:
    virtual int main_fiber() override;
};

// virtual protected
int Application::main_fiber()
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


int main(int argc, char* argv[])
{
    bifang::Application* app = new bifang::Application("./configs");
    if (app->init(argc, argv))
        return app->run();

    return 0;
}
