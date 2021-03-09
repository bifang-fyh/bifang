/**
 * brief: http解析简单测试
 */
#include "bifang.h"
 
RootLogger();

static const char request_data[] =
    "POST / HTTP/1.1\r\n"
    "Host: www.bifang.com\r\n"
    "Content-Length: 25\r\n"
    "\r\n"
    "It is a http request test";

// 分批解析
void test_request_parse1()
{
    bifang::http::HttpRequestParser parser;
    std::string tmp = request_data;
    size_t s1 = parser.execute(&tmp[0], 19, 0);
    log_info << "first execute ret=" << s1
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();

    size_t s2 = parser.execute(&tmp[0], tmp.size(), s1);
    log_info << "second execute ret=" << s2
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();
    parser.getData()->setContent(std::string(tmp.begin() + s1 + s2, tmp.end()));
    log_info << std::endl << parser.getData()->toString();
}

// 一次性解析
void test_request_parse2()
{
    bifang::http::HttpRequestParser parser;
    std::string tmp = request_data;
    size_t s = parser.execute(&tmp[0], tmp.size(), 0);
    log_info << "execute ret=" << s
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();
    std::string body(tmp.begin() + s, tmp.end());
    parser.getData()->setContent(body);
    log_info << std::endl << parser.getData()->toString();
}

static const char response_data[] =
    "HTTP/1.1 200 OK\r\n"
    "Server: bifang\r\n"
    "Date: Thu, 16 Jan 2020 00:41:21 GMT\r\n"
    "Content-Type: video/x-flv\r\n"
    "Content-Length: 6\r\n"
    "Last-Modified: Tue, 08 Jan 2019 20:40:34 GMT\r\n"
    "Connection: keep-alive\r\n"
    "Expires: Thu, 31 Dec 2037 23:55:55 GMT\r\n"
    "Cache-Control: max-age=315360000\r\n"
    "Accept-Ranges: bytes\r\n"
    "\r\n"
    "111111";

// 分批解析
void test_response_parse1()
{
    bifang::http::HttpResponseParser parser;
    std::string tmp = response_data;
    size_t s1 = parser.execute(&tmp[0], 27, 0, false);
    log_info << "execute ret=" << s1
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();

    size_t s2 = parser.execute(&tmp[0], tmp.size(), s1, false);
    log_info << "execute ret=" << s2
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();
    parser.getData()->setContent(std::string(tmp.begin() + s1 + s2, tmp.end()));
    log_info << std::endl << parser.getData()->toString();
}

// 一次性解析
void test_response_parse2()
{
    bifang::http::HttpResponseParser parser;
    std::string tmp = response_data;
    size_t s = parser.execute(&tmp[0], tmp.size(), 0, true);
    log_info << "execute ret=" << s
        << ", has_error=" << parser.hasError()
        << ", is_finished=" << parser.isFinished()
        << ", total=" << tmp.size()
        << ", content_length=" << parser.getContentLength();
    std::string body(tmp.begin() + s, tmp.end());
    parser.getData()->setContent(body);
    log_info << std::endl << parser.getData()->toString();
}

int main(int argc, char* argv[])
{
    test_request_parse1();
    std::cout << "--------------------------------------" << std::endl;
    test_request_parse2();

    std::cout << std::endl << std::endl;

    test_response_parse1();
    std::cout << "--------------------------------------" << std::endl;
    test_response_parse2();

    return 0;
}
