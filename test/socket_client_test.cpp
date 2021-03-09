/**
 * brief: socket客户端简单测试
 */
#include "bifang.h"

RootLogger();

static bifang::IOManager::ptr iom;

void run()
{
    bifang::Address::ptr address = bifang::Address::create("127.0.0.1", 7477);
    bifang::Socket::ptr sock = bifang::Socket::createTCP(address);
    sock->connect(address);
    unsigned int u32 = htonl(0x11223344);
    int n;
    if ((n = sock->send(&u32, sizeof(u32))) <= 0)
    {
        log_error << "send message to wailian error!";
        return;
    }

    log_info << "send 0x11223344 to server";

    //while (1)
    //{
    //    std::string data;
    //    std::cin >> data;
    //    int n;
    //
    //    if ((n = sock->send(&data[0], data.size())) <= 0)
    //        break;
    //
    //    if ((n = sock->recv(&data[0], data.size())) <= 0)
    //        break;
    //
    //    log_info << "recv from " << *(sock->GetRemoteAddress())
    //        << " data: " << data;
    //}
    //
    //log_info << "remote addr=" << *(sock->GetRemoteAddress())
    //    << " disconnected";
}

int main(int argc, char *argv[])
{
    //iom.reset(new bifang::IOManager(3, false));
    //iom->schedule(std::bind(run, argv[1], type));
    run();

    return 0;
}
