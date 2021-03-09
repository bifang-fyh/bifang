/**
 * brief: websocket客户端
 */
#ifndef __BIFANG_WS_CLIENT_H
#define __BIFANG_WS_CLIENT_H

#include "http/http_common.h"
#include "http/http_client.h"
#include "ws_session.h"


namespace bifang
{

namespace ws
{

/**
 * brief: websocket客户端类
 */
class WSClient : public http::HttpClient
{
public:
    typedef std::shared_ptr<WSClient> ptr;

    WSClient(Socket::ptr sock);

    /**
     * brief: 接收websocket消息
     * return: 接收到的消息
     */
    WSFrameMessage::ptr recvMessage();
    /**
     * brief: 发送websocket消息
     * param: msg 需要发送的消息
     *        fin 为true代表该帧为结束帧
     * return: 发送的数据长度
     */
    int32_t sendMessage(WSFrameMessage::ptr msg, bool fin = true);
    /**
     * brief: 发送websocket消息
     * param: msg 需要发送的消息的字符串
     *        opcode 操作码
     *        fin 为true代表该帧为结束帧
     * return: 发送的数据长度
     */
    int32_t sendMessage(const std::string& msg,
                int32_t opcode = WSFrameHead::TEXT_FRAME, bool fin = true);

    int32_t ping();
    int32_t pong();

public:
    static std::pair<http::HttpResult::ptr, WSClient::ptr> create(Uri::ptr uri,
        int64_t timeout_ms,
        const std::map<std::string, std::string, http::CaseInsensitive>& headers = {});

    static std::pair<http::HttpResult::ptr, WSClient::ptr> create(const std::string& url,
        int64_t timeout_ms,
        const std::map<std::string, std::string, http::CaseInsensitive>& headers = {});
};

}

}

#endif /*__BIFANG_WS_CLIENT_H*/
