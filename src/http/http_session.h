/**
 * brief: http session
 */
#ifndef __BIFANG_HTTP_SESSION_H
#define __BIFANG_HTTP_SESSION_H

#include "stream/socket_stream.h"
#include "http_common.h"


namespace bifang
{

namespace http
{

/**
 * brief: http session类
 */
class HttpSession : public SocketStream
{
public:
    typedef std::shared_ptr<HttpSession> ptr;

    /**
     * brief: 构造函数
     * param: sock Socket类型
     */
    HttpSession(Socket::ptr sock);

    /**
     * brief: 接收HTTP请求
     * return: http请求的智能指针
     */
    HttpRequest::ptr recvRequest();

    /**
     * brief: 发送HTTP响应
     * param: rsp HTTP响应
     * return: >0 发送成功
     *         =0 对方关闭
     *         <0 Socket异常
     */
    int64_t sendResponse(HttpResponse::ptr rsp);
};

}

}

#endif /*__BIFANG_HTTP_SESSION_H*/
