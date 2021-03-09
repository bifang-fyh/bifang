#include "bifang.h"

RootLogger();

class EchoServer : public bifang::TcpServer
{
public:
    EchoServer() {}

    void handleClient(bifang::Socket::ptr client) override;
};

void EchoServer::handleClient(bifang::Socket::ptr client)
{
    log_info << "handleClient " << *client;
    bifang::Buffer::ptr buf(new bifang::Buffer);

    while (1)
    {
        //if (buf->getSize() > 1024)
        //    buf->clear();
        std::vector<iovec> iovs;
        buf->getWriteBuffers(iovs, 1024);

        int ret = client->recv(&iovs[0], iovs.size());
        if (ret == 0)
        {
            log_info << "client close: " << *client;
            break;
        }
        else if (ret < 0)
        {
            log_info << "client error ret=" << ret
                << ", errno=" << errno << ", errstr=" << strerror(errno);
            break;
        }
        //size_t pos = buf->getPosition();
        //buf->setPosition(pos + ret);
        //buf->setPosition(pos);
        //log_info << "buf size=" << buf->getSize();
        //log_info << "first buf=" << buf->readInt((uint32_t)1);
        //log_info << "second buf=" << buf->readInt((uint32_t)1);
    }
}

void run()
{
    EchoServer::ptr es(new EchoServer);
    auto addr = bifang::Address::getAddrInfo("0.0.0.0:8999");
    while (!es->bind(addr))
    {
        log_warn << "bind error, please wait 2s ";
        sleep(2);
    }
    es->start();
}

int main(int argc, char* argv[])
{
    bifang::ConfigMgr::GetInstance()->load("./configs/");
    bifang::IOManager iom(10, false, "test");
    iom.schedule(run);
    return 0;
}
