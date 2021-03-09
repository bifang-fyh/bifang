#include "ws_session.h"
#include "config.h"
#include "endian_cpp.h"


namespace bifang
{

namespace ws
{

SystemLogger();

static Config<uint32_t>::ptr g_websocket_message_max_size =
    ConfigMgr::GetInstance()->get("websocket.message.max_size", (uint32_t)32 * 1024 * 1024, "websocket message max size");


WSSession::WSSession(Socket::ptr sock)
    :http::HttpSession(sock)
{
}

http::HttpRequest::ptr WSSession::handleShake()
{
#define XX(message) \
    log_error << message; \
    if (req) \
        log_info << req; \
    return nullptr

    http::HttpRequest::ptr req = recvRequest();
    if (!req)
    {
        XX("invalid http request");
    }
    if (strcasecmp(req->getHeader("Upgrade").c_str(), "websocket"))
    {
        XX("http header Upgrade != websocket");
    }
    if (strcasecmp(req->getHeader("Connection").c_str(), "Upgrade"))
    {
        XX("http header Connection != Upgrade");
    }
    if (req->getHeaderAs<int>("Sec-webSocket-Version") != 13)
    {
        XX("http header Sec-webSocket-Version != 13");
    }
    std::string key = req->getHeader("Sec-WebSocket-Key");
    if (key.empty())
    {
        XX("http header Sec-WebSocket-Key = null");
    }

    std::string v = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    v = bifang::CryptUtil::base64_encode(bifang::CryptUtil::sha1sum(v));
    req->setWebsocket(true);

    auto rsp = req->createResponse();
    rsp->setStatus(http::HttpStatus::SWITCHING_PROTOCOLS);
    rsp->setWebsocket(true);
    rsp->setReason("Web Socket Protocol Handshake");
    rsp->setHeader("Upgrade", "websocket");
    rsp->setHeader("Connection", "Upgrade");
    rsp->setHeader("Sec-WebSocket-Accept", v);
    sendResponse(rsp);

    return req;
#undef XX
}

WSFrameMessage::ptr WSSession::recvMessage()
{
    return WSRecvMessage(this, false);
}

int32_t WSSession::sendMessage(WSFrameMessage::ptr msg, bool fin)
{
    return WSSendMessage(this, msg, false, fin);
}

int32_t WSSession::sendMessage(const std::string& msg, int32_t opcode, bool fin)
{
    return WSSendMessage(this, std::make_shared<WSFrameMessage>(opcode, msg),
               false, fin);
}

int32_t WSSession::ping()
{
    return WSPing(this);
}

int32_t WSSession::WSSession::pong()
{
    return WSPong(this);
}


// global
WSFrameMessage::ptr WSRecvMessage(Stream* stream, bool is_client)
{
    int opcode = 0;
    std::string data;
    int cur_len = 0;
    while (1)
    {
        WSFrameHead ws_head;
        if (stream->readFixSize(&ws_head, sizeof(ws_head)) <= 0)
        {
            log_error << "recv WSFrameHead error";
            break;
        }
        log_debug << "WSFrameHead " << ws_head.toString();

        if (ws_head.opcode == WSFrameHead::PING)
        {
            log_debug << "PING";
            if (WSPong(stream) <= 0)
                break;
        }
        else if (ws_head.opcode == WSFrameHead::PONG)
        {
            log_debug << "PONG";
        }
        else if (ws_head.opcode == WSFrameHead::CLOSE)
        {
            return WSFrameMessage::ptr(new WSFrameMessage(WSFrameHead::CLOSE, ""));
        }
        else if (ws_head.opcode == WSFrameHead::CONTINUE
              || ws_head.opcode == WSFrameHead::TEXT_FRAME
              || ws_head.opcode == WSFrameHead::BIN_FRAME)
        {
            // 表示当前作为服务器接收客户端消息, 客户端发送的数据必须经过掩码处理
            if (!is_client && !ws_head.mask)
            {
                log_error << "recv from client WSFrameHead mask != 1";
                break;
            }
            uint64_t length = 0;
            if (ws_head.payload == 126)
            {
                uint16_t len = 0;
                if (stream->readFixSize(&len, sizeof(len)) <= 0)
                {
                    log_error << "read 16bit length data error";
                    break;
                }
                length = bifang::littleByteSwap(len);
            }
            else if (ws_head.payload == 127)
            {
                uint64_t len = 0;
                if (stream->readFixSize(&len, sizeof(len)) <= 0)
                {
                    log_error << "read 32bit length data error";
                    break;
                }
                length = bifang::littleByteSwap(len);
            }
            else
            {
                length = ws_head.payload;
            }

            if ((cur_len + length) >= g_websocket_message_max_size->getValue())
            {
                log_warn << "WSFrameMessage length > "
                    << g_websocket_message_max_size->getValue()
                    << " (" << (cur_len + length) << ")";
                break;
            }

            char mask[4] = {0};
            if (ws_head.mask)
            {
                if (stream->readFixSize(mask, sizeof(mask)) <= 0)
                {
                    log_error << "read masking key error";
                    break;
                }
            }
            data.resize(cur_len + length);
            if (stream->readFixSize(&data[cur_len], length) <= 0)
            {
                log_error << "read data error";
                break;
            }
            if (ws_head.mask)
            {
                for (size_t i = 0; i < length; i++)
                    data[cur_len + i] ^= mask[i % 4];
            }
            cur_len += length;

            if (!opcode && ws_head.opcode != WSFrameHead::CONTINUE)
                opcode = ws_head.opcode;

            if (ws_head.fin)
            {
                log_debug << data;
                return WSFrameMessage::ptr(new WSFrameMessage(opcode, std::move(data)));
            }
        }
        else
            log_error << "invalid opcode=" << ws_head.opcode;
    };
    stream->close();
    return nullptr;
}

// global
int32_t WSSendMessage(Stream* stream, WSFrameMessage::ptr msg,
            bool is_client, bool fin)
{
#define XX(message) \
    log_error << message; \
    stream->close(); \
    return -1
    
    WSFrameHead ws_head;
    memset(&ws_head, 0, sizeof(ws_head));
    ws_head.fin = fin;
    ws_head.opcode = msg->getOpcode();
    ws_head.mask = is_client;
    uint64_t size = msg->getData().size();
    if (size < 126)
        ws_head.payload = size;
    else if (size < 65536)
        ws_head.payload = 126;
    else
        ws_head.payload = 127;

    if (stream->writeFixSize(&ws_head, sizeof(ws_head)) <= 0)
    {
        XX("send WSFrameHead error");
    }

    if (ws_head.payload == 126)
    {
        uint16_t len = size;
        len = bifang::littleByteSwap(len);
        if (stream->writeFixSize(&len, sizeof(len)) <= 0)
        {
            XX("write 16bit length data error");
        }
    }
    else if (ws_head.payload == 127)
    {
        uint64_t len = bifang::littleByteSwap(size);
        if (stream->writeFixSize(&len, sizeof(len)) <= 0)
        {
            XX("write 32bit length data error");
        }
    }

    if (is_client)
    {
        char mask[4];
        static int s = Srand();
        uint32_t rand_value = rand();
        memcpy(mask, &rand_value, sizeof(mask));
        std::string& data = msg->getData();
        for (size_t i = 0; i < data.size(); i++)
        {
            data[i] ^= mask[i % 4];
        }

        if (stream->writeFixSize(mask, sizeof(mask)) <= 0)
        {
            XX("write masking key error");
        }
    }
    if (stream->writeFixSize(msg->getData().c_str(), size) <= 0)
    {
        XX("write data error");
    }

    return size + sizeof(ws_head);
#undef XX
}

// global
int32_t WSPing(Stream* stream)
{
    WSFrameHead ws_head;
    memset(&ws_head, 0, sizeof(ws_head));
    ws_head.fin = 1;
    ws_head.opcode = WSFrameHead::PING;
    int32_t v = stream->writeFixSize(&ws_head, sizeof(ws_head));
    if (v <= 0)
    {
        log_warn << stream->toString() << std::endl << "websocket ping error!";
        stream->close();
    }
    return v;
}

// global
int32_t WSPong(Stream* stream)
{
    WSFrameHead ws_head;
    memset(&ws_head, 0, sizeof(ws_head));
    ws_head.fin = 1;
    ws_head.opcode = WSFrameHead::PONG;
    int32_t v = stream->writeFixSize(&ws_head, sizeof(ws_head));
    if (v <= 0)
    {
        log_warn << stream->toString() << std::endl << "websocket pong error!";
        stream->close();
    }
    return v;
}

}

}
