/**
 * brief: socket服务端简单测试
 */
#include "bifang.h"
 
RootLogger();

static bifang::IOManager::ptr iom;

void handle(bifang::Socket::ptr accept)
{
    log_info << "remote addr=" << *(accept->getRemoteAddress())
        << " connected successful";

    while (1)
    {
        std::string data;
        data.resize(1024);
        int n;
        if ((n = accept->recv(&data[0], data.size())) <= 0)
            break;
        if (n != 4)
        {
            log_warn << "data size is invaild";
            continue;
        }
        data.resize(n);
        unsigned int u32 = 0;
        memcpy(&u32, data.c_str(), 4);
        log_info << "recv from " << *(accept->getRemoteAddress())
            << std::endl << "data: " << std::hex << ntohl(u32) << std::dec;
    }

    log_info << "remote addr=" << *(accept->getRemoteAddress())
        << " disconnected";
}

void run()
{
    bifang::Address::ptr address = bifang::Address::create("0.0.0.0", 7477);
    bifang::Socket::ptr sock = bifang::Socket::createTCP(address);
    sock->bind(address);
    sock->listen();

    while (1)
    {
        bifang::Socket::ptr accept = sock->accept();
        iom->schedule(std::bind(&handle, accept));
    }
}

int main(int argc, char *argv[])
{
    iom.reset(new bifang::IOManager(3, false));
    iom->schedule(run);

    return 0;
}
