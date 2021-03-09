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
    bool init(int argc, char** argv);
    bool run();

public:
    void startServer(std::vector<TcpServer::ptr>& servers);

    bool getServer(const std::string& type, std::vector<TcpServer::ptr>& servers);

    std::map<std::string, std::vector<TcpServer::ptr> >& GetListOfServer();

    IOManager::ptr getMainIOManager() const { return m_mainIOManager; }

private:
    int main(int argc, char** argv);
    int main_fiber();

private:
    int m_argc = 0;
    char** m_argv = nullptr;
    bool m_running = false;
    std::map<std::string, std::vector<TcpServer::ptr> > m_servers;
    IOManager::ptr m_mainIOManager;
};

typedef Singleton<ServerManager> ServerMgr;

}

#endif /*__BIFANG_SERVER_H*/
