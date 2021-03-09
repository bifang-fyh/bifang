/**
 * brief: websocket session
 */
#ifndef __BIFANG_WS_SESSION_H
#define __BIFANG_WS_SESSION_H

#include <string.h>
#include <stdlib.h>

#include "http/http_session.h"
#include "util.h"


namespace bifang
{

namespace ws
{

/**
 * brief: websocket帧头封装
 */
/*--------------------------------------------------------------
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
-----------------------------------------------------------------*/
#pragma pack(1)
struct WSFrameHead
{
    enum Opcode
    {
        // 数据分片帧
        CONTINUE = 0x00,
        // 文本帧
        TEXT_FRAME = 0x01,
        // 二进制帧
        BIN_FRAME = 0x02,
        // 断开连接
        CLOSE = 0x08,
        // PING
        PING = 0x09,
        // PONG
        PONG = 0x0A
    };
    uint32_t opcode: 4;
    bool rsv3: 1;
    bool rsv2: 1;
    bool rsv1: 1;
    bool fin: 1;
    uint32_t payload: 7;
    // 浏览器发送的mask必为1, 服务器发送的帧必为0, 否则应断开WebSocket连接
    bool mask: 1; 

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[WSFrameHead fin=" << fin
           << ", rsv1=" << rsv1
           << ", rsv2=" << rsv2
           << ", rsv3=" << rsv3
           << ", opcode=" << opcode
           << ", mask=" << mask
           << ", payload=" << payload
           << "]";
        return ss.str();
    }
};
#pragma pack()

/**
 * brief: websocket消息体封装
 */
class WSFrameMessage
{
public:
    typedef std::shared_ptr<WSFrameMessage> ptr;

    WSFrameMessage(int opcode = 0, const std::string& data = "")
        :m_opcode(opcode)
        ,m_data(data)
    {
    }

    int getOpcode()              const { return m_opcode; }
    std::string& getData()             { return m_data;   }
    const std::string& getData() const { return m_data;   }
    
    void setOpcode(int v)              { m_opcode = v;    }
    void setData(const std::string& v) { m_data = v;      }

private:
    // 操作代码
    int m_opcode;
    // 消息内容
    std::string m_data;
};

/**
 * brief: websocket session封装
 */
class WSSession : public http::HttpSession
{
public:
    typedef std::shared_ptr<WSSession> ptr;

    WSSession(Socket::ptr sock);

    /**
     * brief: websocket握手(server client共用)
     * return: 成功返回一个HttpRequest, 失败返回nullptr
     */
    http::HttpRequest::ptr handleShake();

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
};

WSFrameMessage::ptr WSRecvMessage(Stream* stream, bool is_client);
int32_t WSSendMessage(Stream* stream, WSFrameMessage::ptr msg,
            bool is_client, bool fin);
int32_t WSPing(Stream* stream);
int32_t WSPong(Stream* stream);

}

}

#endif /*__BIFANG_WS_SESSION_H*/
