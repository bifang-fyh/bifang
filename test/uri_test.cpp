/**
 * brief: uri解析简单测试
 */
#include "bifang.h"


void uri_test(const std::string& uristr)
{
    bifang::Uri::ptr uri = bifang::Uri::create(uristr);
    std::cout << uri->toString() << std::endl;
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
