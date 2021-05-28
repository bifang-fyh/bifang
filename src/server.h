#ifndef __BIFANG_SERVER_H
#define __BIFANG_SERVER_H

#include <map>

#include "iomanager.h"
#include "http/http_server.h"

namespace bifang
{

class ServerManager
{
public:
    ServerManager(const std::string& config_path);

    virtual ~ServerManager() {}

    bool init(int argc, char** argv);
    bool run();

private:
    int main(int argc, char** argv);

    virtual int main_fiber() = 0;

protected:
    int m_argc = 0;
    char** m_argv = nullptr;
    bool m_running = false;
    std::string m_config_path;
    IOManager::ptr m_mainIOManager;
};

extern std::map<std::string, std::vector<TcpServer::ptr> > g_servers;

bool getServer(const std::string& type, std::vector<TcpServer::ptr>& servers);
const std::map<std::string, std::vector<TcpServer::ptr> >& getListOfServer();

}

#endif /*__BIFANG_SERVER_H*/
