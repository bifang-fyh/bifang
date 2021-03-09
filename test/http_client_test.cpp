/**
 * brief: http客户端简单测试
 */
#include "bifang.h"

RootLogger();

void test1()
{
    auto res = bifang::http::HttpClient::DoGet("http://www.baidu.com:80/s?wd=今日新鲜事&tn=SE_Pclogo_6ysd4c7a&sa=ire_dl_gh_logo&rsv_dl=igh_logo_pc", 300,
            {
                {"Accept-Encoding", "gzip, deflate, br"},
                {"Connection", "keep-alive"},
            });
    log_info << std::endl << res;
}

void test2()
{
    bifang::Address::ptr addr = bifang::Address::getAddrInfo("www.baidu.com:80");
    if (!addr)
    {
        log_error << "get addr error";
        return;
    }

    bifang::Socket::ptr sock = bifang::Socket::createTCP(addr);
    bool ret = sock->connect(addr);
    if (!ret)
    {
        log_info << "connect " << *addr << " failed";
        return;
    }

    bifang::http::HttpClient::ptr conn(new bifang::http::HttpClient(sock));
    bifang::http::HttpRequest::ptr req(new bifang::http::HttpRequest(false));
    req->setHeader("host", "www.baidu.com");
    req->setPath("/s");
    req->setQuery("wd=今日新鲜事&tn=SE_Pclogo_6ysd4c7a&sa=ire_dl_gh_logo&rsv_dl=igh_logo_pc");
    log_info << "req:" << std::endl << req;
    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        log_info << "recv response error";
        return;
    }
    log_info << "rsp:" << std::endl << rsp;

    std::ofstream ofs;
    if (!bifang::FileUtil::OpenForWrite(ofs, "logs/baidu.txt", std::ios::trunc))
    {
        log_info << "open logs/baidu.txt error";
        return;
    }
    ofs << rsp;
}


int main(int argc, char* argv[])
{
    bifang::IOManager iom(2);
    iom.schedule(test1);
    //iom.schedule(test2);

    return 0;
}
