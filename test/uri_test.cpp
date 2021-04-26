/**
 * brief: uri解析简单测试
 */
#include "bifang.h"

void uri_test(const std::string& uristr)
{
    std::cout << "start parse uri:" << uristr << std::endl;
    Uri::ptr uri = Uri::create(uristr);
    if (uri)
    {
        std::cout << "uri parse successful" << std::endl;
        std::cout << "scheme:" << uri->getScheme() << std::endl
                  << "userinfo:" << uri->getUserinfo() << std::endl
                  << "host:" << uri->getHost() << std::endl
                  << "port:" << uri->getPort() << std::endl
                  << "path:" << uri->getPath() << std::endl
                  << "query:" << uri->getQuery() << std::endl
                  << "fragment:" << uri->getFragment() << std::endl;
    }
    else
        std::cout << "uri parse fail" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    uri_test("http://admin@www.bifang.com");
    uri_test("http://www.bifang.com/test/uri?id=88&name=bifang#fragment");
    uri_test("foo://user@example.com:8042/over/there?name=ferret#nose");
    uri_test("http://admin@www.bifang.com/test/中文/uri?id=88&name=bifang&language=中文#frg中文");
    uri_test("https://www.baidu.com");
    uri_test("//www.baidu.com/path/hehe");
    uri_test("//www.bifang.com/test/uri?id=88&name=bifang#fragment");
    uri_test("http://192.168.0.1/test/uri?id=88&name=bifang#fragment");
    uri_test("/path/bifang/picture/");

    return 0;
}
