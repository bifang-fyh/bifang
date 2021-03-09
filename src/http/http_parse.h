#ifndef __BIFANG_HTTP_PARSE_H
#define __BIFANG_HTTP_PARSE_H

#include "http_server_parse.h"
#include "http_client_parse.h"

namespace bifang
{

namespace http
{

/**
 * brief: HTTP请求解析类
 */
class HttpRequestParser
{
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;

    HttpRequestParser();

    /**
     * brief: 解析协议
     * param: data 协议文本内存
     *        len 协议文本内存长度
     * return: 返回实际解析的长度, 并且将已解析的数据移除
     */
    size_t execute(char* data, size_t len, size_t off);

    /**
     * brief: 是否解析完成
     * return: -1 - 解析出现错误
     *         1  - 解析已完成
     *         0  - 解析未完成未出现无错误
     */
    int isFinished() { return http_server_parser_finish(&m_parser); }

    /**
     * brief: 是否有错误
     * return: 0    - 成功
     *         其他 - 失败
     */
    int hasError() { return m_error || http_server_parser_has_error(&m_parser); }

    /**
     * brief: 返回HttpRequest结构体
     */
    HttpRequest::ptr getData() const { return m_data; }

    /**
     * brief: 获取消息体长度
     */
    uint64_t getContentLength() const
    {
        return m_data->getHeaderAs<uint64_t>("content-length", 0);
    }

    /**
     * brief: 获取http_parser结构体
     */
    const http_server_parser& getParser() const { return m_parser; }

    /**
     * brief: 设置错误
     * param: v 错误值
     */
    void setError(int v) { m_error = v; }

public:
    /**
     * brief: 返回HttpRequest协议解析的缓存大小
     */
    static uint64_t getHttpRequestBufferSize();

private:
    http_server_parser m_parser;
    HttpRequest::ptr m_data;
    // 错误码
    // 1000: invalid method
    // 1001: invalid version
    int m_error = 0;
};

/**
 * brief: HTTP响应解析结构体
 */
class HttpResponseParser
{
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;

    HttpResponseParser();

    /**
     * brief: 解析HTTP响应协议
     * param: data 协议数据内存
     *        len 协议数据内存大小
     *        chunck 是否在解析chunck
     * return: 返回实际解析的长度, 并且移除已解析的数据
     */
    int execute(char* data, size_t len, size_t off, bool chunck);

    /**
     * brief: 是否解析完成
     * return: -1 - 解析出现错误
     *         1  - 解析已完成
     *         0  - 解析未完成未出现无错误
     */
    int isFinished() { http_client_parser_finish(&m_parser); }

    /**
     * brief: 是否有错误
     * return: 0    - 成功
     *         其他 - 失败
     */
    int hasError() { return m_error || http_client_parser_has_error(&m_parser); }

    /**
     * brief: 返回HttpResponse
     */
    HttpResponse::ptr getData() const { return m_data; }
    
    /**
     * brief: 获取消息体长度
     */
    uint64_t getContentLength() const
    {
        return m_data->getHeaderAs<uint64_t>("content-length", 0);
    }

    /**
     * brief: 返回httpclient_parser
     */
    const http_client_parser& getParser() const { return m_parser; }

    /**
     * brief: 设置错误码
     * param: v 错误码
     */
    void setError(int v) { m_error = v; }

public:
    /**
     * brief: 返回HTTP响应解析缓存大小
     */
    static uint64_t getHttpResponseBufferSize();

private:
    http_client_parser m_parser;
    HttpResponse::ptr m_data;
    // 错误码
    // 1001: invalid version
    int m_error = 0;
};

}

}

#endif /*__BIFANG_HTTP_PARSE_H*/
