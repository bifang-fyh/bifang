/**
 * brief: http请求和响应简单测试
 */
#include "bifang.h"

void test_request()
{
    // 内对外请求
    std::cout << "------request server begin------" << std::endl;
    bifang::http::HttpRequest::ptr req1(new bifang::http::HttpRequest(false));
    req1->setQuery("name=bifang&age=25&性别=男&地址=百度");
    req1->setFragment("百度一下, 你就知道");
    req1->setHeader("Host", "www.baidu.com");
    req1->setHeader("Connection", "keep-alive");
    req1->setHeader("姓名", "小红");
    req1->setCookie("11", "222");
    req1->setCookie("姓名", "小黄");
    req1->setCookie("aaa", "2332323");
    req1->setContent("第三方士大夫十多个风格范德萨范德萨1212fsfsdf萨芬`@#$$%");
    std::cout << req1->toString() << std::endl;
    std::cout << "------request server end------" << std::endl << std::endl;

    // 外对内请求
    std::cout << "------request client begin------" << std::endl;
    bifang::http::HttpRequest::ptr req2(new bifang::http::HttpRequest(true));
    req2->setQuery("%E5%9C%B0%E5%9D%80=%E7%99%BE%E5%BA%A6&%E6%80%A7%E5%88%AB=%E7%94%B7&age=25&name=bifang");
    req2->setFragment("%E7%99%BE%E5%BA%A6%E4%B8%80%E4%B8%8B%2C+%E4%BD%A0%E5%B0%B1%E7%9F%A5%E9%81%93");
    req2->setHeaderMap({
        {"host", "www.baidu.com"},
        {"connection", "keep-alive"},
        {"姓名", "小红"},
        {"Cookie", "11=222&姓名=小黄"}
        });
    req2->setContent("第三方士大夫十多个风格范德萨范德萨1212fsfsdf萨芬`@#$$%");
    std::cout << req2->toString() << std::endl;
    std::cout << "------request client end------" << std::endl;
}

void test_response()
{
    // 内对外响应
    std::cout << "------response server begin------" << std::endl;
    bifang::http::HttpResponse::ptr rsp1(new bifang::http::HttpResponse(false));
    rsp1->setHeader("haha", "11111");
    rsp1->setHeader("姓名", "小红");
    rsp1->setHeader("Set-Cookie", "1111");
    rsp1->setHeader("Set-Cookie", "2222");
    rsp1->setHeader("set-cookie", "大红");
    rsp1->setStatus((bifang::http::HttpStatus)200);
    rsp1->setClose(true);
    rsp1->setContent("Hi, my name is bifang, how old are you, what's you name! 你好, 我是...");
    std::cout << rsp1->toString() << std::endl;
    std::cout << "------response server end------" << std::endl << std::endl;

    // 外对内响应
    std::cout << "------response client begin------" << std::endl;
    bifang::http::HttpResponse::ptr rsp2(new bifang::http::HttpResponse(true));
    rsp2->setHeaderMap({
        {"haha", "11111"},
        {"姓名", "小红"},
        {"Set-Cookie", "1111"},
        {"Set-Cookie", "2222"},
        {"Set-Cookie", "大红"},
        {"connection", "keep-alive"}
        });
    rsp2->setStatus((bifang::http::HttpStatus)200);
    rsp2->setContent("Hi, my name is bifang, how old are you, what's you name! 你好, 我是...");
    std::cout << rsp2->toString() << std::endl;
    std::cout << "------response client end------" << std::endl;
}

int main(int argc, char* argv[])
{
    test_request();
    std::cout << std::endl << std::endl;
    test_response();

    return 0;
}
