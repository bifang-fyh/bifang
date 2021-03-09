#include "ws_client.h"
#include "log.h"

namespace bifang
{

namespace ws
{

SystemLogger();

WSClient::WSClient(Socket::ptr sock)
    :http::HttpClient(sock)
{
}

WSFrameMessage::ptr WSClient::recvMessage()
{
    return WSRecvMessage(this, true);
}

int32_t WSClient::sendMessage(WSFrameMessage::ptr msg, bool fin)
{
    return WSSendMessage(this, msg, true, fin);
}

int32_t WSClient::sendMessage(const std::string& msg, int32_t opcode, bool fin)
{
    return WSSendMessage(this, std::make_shared<WSFrameMessage>(opcode, msg),
               true, fin);
}

int32_t WSClient::ping()
{
    return WSPing(this);
}

int32_t WSClient::pong()
{
    return WSPong(this);
}

// static
std::pair<http::HttpResult::ptr, WSClient::ptr> WSClient::create(Uri::ptr uri,
    int64_t timeout_ms, const std::map<std::string, std::string, http::CaseInsensitive>& headers)
{
    Address::ptr addr = uri->createAddress();
    if (!addr)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::INVALID_HOST,
                   nullptr, "invalid host: " + uri->getHost()), nullptr);
    }
    bool is_ssl = uri->getScheme() == "wss";
    Socket::ptr sock = is_ssl ? SSLSocket::createTCP(addr) : Socket::createTCP(addr);
    if (!sock)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::CREATE_SOCKET_ERROR,
                   nullptr, "create socket fail: " + addr->toString()
                   + ", errno=" + std::to_string(errno)
                   + ", errstr=" + std::string(strerror(errno))), nullptr);
    }
    if (!sock->connect(addr))
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::CONNECT_FAIL,
                   nullptr, "connect fail: " + addr->toString()), nullptr);
    }
    sock->setRecvTimeout(timeout_ms);
    WSClient::ptr conn = std::make_shared<WSClient>(sock);
    http::HttpRequest::ptr req = std::make_shared<http::HttpRequest>(false);
    req->setMethod(http::HttpMethod::GET);
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());
    req->setHeaderMap(headers);
    req->setWebsocket(true);
    if (req->getHeader("Host").empty())
        req->setHeader("Host", uri->getHost());
    if (req->getHeader("Connection").empty())
        req->setHeader("Connection", "Upgrade");
    req->setHeader("Upgrade", "websocket");
    req->setHeader("Sec-webSocket-Version", "13");
    req->setHeader("Sec-webSocket-Key", bifang::CryptUtil::base64_encode(bifang::StringUtil::randomString(16)));
    //log_debug << std::endl << req;

    int ret = conn->sendRequest(req);
    if (ret == 0)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::SEND_CLOSE_BY_PEER,
                   nullptr, "send request closed by peer: " + addr->toString()), nullptr);
    }
    else if (ret < 0)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::SEND_SOCKET_ERROR,
                   nullptr, "send request socket error, errno=" + std::to_string(errno)
                   + ", errstr=" + std::string(strerror(errno))), nullptr);
    }

    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::TIMEOUT,
                   nullptr, "recv response timeout: " + addr->toString()
                   + " timeout_ms:" + std::to_string(timeout_ms)), nullptr);
    }

    if (rsp->getStatus() != http::HttpStatus::SWITCHING_PROTOCOLS)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::WS_INVALID_Status,
                   rsp, "not websocket server " + addr->toString()), nullptr);
    }
    return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::OK,
               rsp, "ok"), conn);
}

// static
std::pair<http::HttpResult::ptr, WSClient::ptr> WSClient::create(const std::string& url,
    int64_t timeout_ms, const std::map<std::string, std::string, http::CaseInsensitive>& headers)
{
    Uri::ptr uri = Uri::create(url);
    if (!uri)
    {
        return std::make_pair(std::make_shared<http::HttpResult>((int)http::HttpResult::Error::INVALID_URL,
                   nullptr, "invalid url:" + url), nullptr);
    }
    return create(uri, timeout_ms, headers);
}

}

}
