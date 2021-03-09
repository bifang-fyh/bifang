/**
 * brief: hook简单测试
 */
#include "bifang.h"

 
RootLogger();

void test_sleep()
{
    bifang::IOManager iom(1);
    iom.schedule(
        []()
        {
                sleep(2);
                log_info << "sleep 2";
        });

    iom.schedule(
        []()
        {
                sleep(3);
                log_info << "sleep 3";
        });
    log_info << "test_sleep";
}


void test_sock()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "123.57.17.199", &addr.sin_addr.s_addr);

    log_info << "begin connect";
    int ret = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    log_info << "connect ret=" << ret << " errno=" << errno;

    if (ret)
        return;

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    ret = send(sock, data, sizeof(data), 0);
    log_info << "send ret=" << ret << " errno=" << errno;

    if (ret <= 0)
        return;

    std::string buff;
    buff.resize(4096);

    ret = recv(sock, &buff[0], buff.size(), 0);
    log_info << "recv ret=" << ret << " errno=" << errno;

    if (ret <= 0)
        return;

    buff.resize(ret);
    log_info << std::endl << buff;
}

int main(int argc, char* argv[])
{
    test_sleep();
    //bifang::IOManager iom;
    //iom.schedule(test_sock);
    return 0;
}
