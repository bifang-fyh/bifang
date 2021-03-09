/**
 * brief: HTTP客户端封装
 */
#ifndef __BIFANG_HTTP_CLIENT_H
#define __BIFANG_HTTP_CLIENT_H

#include <memory>
#include <string>
#include <sstream>

#include "http_parse.h"
#include "stream/socket_stream.h"
#include "uri.h"

namespace bifang
{

namespace http
{

/**
 * brief: HTTP响应结果
 */
struct HttpResult
{
    typedef std::shared_ptr<HttpResult> ptr;

    /**
     * brief: 错误码
     */
    enum class Error
    {
        // 正常
        OK = 0,
        // 非法URL
        INVALID_URL = 1,
        // 无法解析HOST
        INVALID_HOST = 2,
        // 连接失败
        CONNECT_FAIL = 3,
        // 连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        // 发送请求产生Socket错误
        SEND_SOCKET_ERROR = 5,
        // 超时
        TIMEOUT = 6,
        // 创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        // 从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        // 无效的连接
        POOL_INVALID_CONNECTION = 9,
        // 无效的websocket状态码
        WS_INVALID_Status = 50
    };

    /**
     * brief: 构造函数
     * param: result 错误码
     *        response HTTP响应结构体
     *        error 错误描述
     */
    HttpResult(int result, HttpResponse::ptr response, const std::string& error)
        :m_result(result)
        ,m_response(response)
        ,m_error(error)
    {
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[HttpResult result=" << m_result
           << ", error=" << m_error
           << ", response=" << (m_response ? "\n" + m_response->toString() : "nullptr")
           << "]";
        return ss.str();
    }

public:
    // 错误码
    int m_result;
    // HTTP响应结构体
    HttpResponse::ptr m_response;
    // 错误描述
    std::string m_error;
};


/**
 * brief: HTTP客户端类
 */
class HttpClient : public SocketStream
{
public:
    typedef std::shared_ptr<HttpClient> ptr;

    /**
     * brief: 构造函数
     * param: sock Socket类
     */
    HttpClient(Socket::ptr sock);

    /**
     * brief: 接收HTTP响应
     */
    HttpResponse::ptr recvResponse();

    /**
     * brief: 发送HTTP请求
     * param: req HTTP请求结构
     */
    int sendRequest(HttpRequest::ptr req);

public:
    /**
     * brief: 发送HTTP请求
     * param: method 请求类型
     *        url 请求的url字符串
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method, const std::string& url,
        int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");
    /**
     * brief: 发送HTTP请求
     * param: method 请求类型
     *        uri URI结构体
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method, Uri::ptr uri,
        int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");
    /**
     * brief: 发送HTTP请求
     * param: req 请求结构体
     *        uri URI结构体
     *        timeout_ms 超时时间(ms)
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoRequest(HttpRequest::ptr req, Uri::ptr uri,
        int64_t timeout_ms);

    /**
     * brief: 发送HTTP的GET请求
     * param: url 请求的url字符串
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoGet(const std::string& url, int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");
    /**
     * brief: 发送HTTP的GET请求
     * param: uri URI结构体
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoGet(Uri::ptr uri, int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");

    /**
     * brief: 发送HTTP的POST请求
     * param: url 请求的url字符串
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoPost(const std::string& url, int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");
    /**
     * brief: 发送HTTP的POST请求
     * param: uri URI结构体
     *        timeout_ms 超时时间(ms)
     *        headers HTTP请求头部参数
     *        content 请求消息体
     * return: 返回HTTP响应结果结构体
     */
    static HttpResult::ptr DoPost(Uri::ptr uri, int64_t timeout_ms,
        const std::map<std::string, std::string, CaseInsensitive>& headers = {},
        const std::string& content = "");
};


/**
 * brief: 流式输出HttpResult
 * param: os 输出流
 *        req 响应结果
 * return: 输出流
 */
std::ostream& operator<<(std::ostream& os, const HttpResult::ptr res);

}

}

#endif /*__BIFANG_HTTP_CLIENT_H*/
