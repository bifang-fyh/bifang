#include "http_client.h"
#include "log.h"
#include "stream/zlib_stream.h"

namespace bifang
{

namespace http
{

SystemLogger();

HttpClient::HttpClient(Socket::ptr sock)
    :SocketStream(sock)
{
}

HttpResponse::ptr HttpClient::recvResponse()
{
#define XX(str) \
    log_error << str; \
    close(); \
    delete data; \
    return nullptr

    HttpResponseParser::ptr parser(new HttpResponseParser);
    uint64_t buff_size = HttpResponseParser::getHttpResponseBufferSize();
    char* data = new char[buff_size];
    int data_len = 0;
    int parse_offset = 0;
    do
    {
        int len = read(data + data_len, buff_size - data_len);
        if (len <= 0)
        {
            XX("read response data error!");
        }
        data_len += len;

        int nparse = parser->execute(data, data_len, parse_offset, false);
        if (parser->hasError() || nparse == -1)
        {
            XX("parse response data error!");
        }
        parse_offset += nparse;

        if (parser->isFinished())
            break;

        if (data_len == (int)buff_size) // 超出缓冲区依然没有解析完毕
        {
            XX("http response buffer size is too little!");
        }
    }
    while (1);

    auto& client_parser = parser->getParser();
    std::string content;
    if (client_parser.chunked)
    {
        data_len -= parse_offset;
        memmove(data, data + parse_offset, data_len); // 将已解析的全部移除
        parse_offset = 0;
        do
        {
            bool begin = true;
            do
            {
                if (!begin || data_len == 0)
                {
                    int len = read(data + data_len, buff_size - data_len);
                    if (len <= 0)
                    {
                        XX("read response chunked data error!");
                    }
                    data_len += len;
                }

                int nparse = parser->execute(data, data_len, parse_offset, true);
                if (parser->hasError() || nparse == -1)
                {
                    XX("parse response data error!");
                }
                parse_offset += nparse;

                if (parser->isFinished())
                    break;

                if (data_len == (int)buff_size) // 超出缓冲区依然没有解析完毕
                {
                    XX("http response buffer size is too little!");
                }

                begin = false;
            }
            while (1);

            data_len -= parse_offset;
            memmove(data, data + parse_offset, data_len);
            parse_offset = 0;

            log_debug << "content_len=" << client_parser.content_len;
            if (client_parser.content_len + 2 <= data_len)
            {
                content.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len + 2,
                    data_len - client_parser.content_len - 2);
                data_len -= client_parser.content_len + 2;
            }
            else
            {
                content.append(data, data_len);
                int left = client_parser.content_len - data_len + 2;
                while (left > 0)
                {
                    int ret = read(data, left > (int)buff_size ? (int)buff_size : left);
                    if (ret <= 0)
                    {
                        XX("read response chunked data error!");
                    }
                    content.append(data, ret);
                    left -= ret;
                }
                content.resize(content.size() - 2);
                data_len = 0;
            }
        }
        while (!client_parser.chunks_done);
    }
    else
    {
        int offset = data_len - parse_offset;
        int64_t length = parser->getContentLength();
        if (length > 0)
        {
            content.resize(length);
            int len = 0;
            if (length >= offset)
                len = offset;
            else
                len = length;
            memcpy(&content[0], data, len);
            length -= offset;
            if (length > 0)
            {
                if (readFixSize(&content[len], length) <= 0)
                {
                    XX("read response data error!");
                }
            }
        }
    }

    if (!content.empty())
    {
        auto encoding = parser->getData()->getHeader("content-encoding");
        log_debug << "content-encoding: " << (encoding.empty() ? "none" : encoding)
            << ", size=" << content.size();
        std::string& rsp_content = parser->getData()->getContent();
        if (strcasecmp(encoding.c_str(), "gzip") == 0)
        {
            auto zs = ZlibStream::create(false, ZlibStream::GZIP);
            zs->write(content.c_str(), content.size());
            zs->flush();
            zs->toString().swap(rsp_content);
        }
        else if(strcasecmp(encoding.c_str(), "deflate") == 0)
        {
            auto zs = ZlibStream::create(false, ZlibStream::DEFLATE);
            zs->write(content.c_str(), content.size());
            zs->flush();
            zs->toString().swap(rsp_content);
        }
        else
        {
            content.swap(rsp_content);
        }
    }

    delete data;
    return parser->getData();

#undef XX
}

int HttpClient::sendRequest(HttpRequest::ptr req)
{
    std::string data = req->toString();
    return writeFixSize(data.c_str(), data.size());
}

// static
HttpResult::ptr HttpClient::DoRequest(HttpMethod method,
                                const std::string& url,
                                int64_t timeout_ms,
                                const std::map<std::string, std::string, CaseInsensitive>& headers,
                                const std::string& content)
{
    Uri::ptr uri = Uri::create(url);
    if (!uri)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URL,
                   nullptr, "invalid url: " + url);
    }
    return DoRequest(method, uri, timeout_ms, headers, content);
}
// static
HttpResult::ptr HttpClient::DoRequest(HttpMethod method,
                                Uri::ptr uri,
                                int64_t timeout_ms,
                                const std::map<std::string, std::string, CaseInsensitive>& headers,
                                const std::string& content)
{
    HttpRequest::ptr req = std::make_shared<HttpRequest>(false);
    req->setMethod(method);
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());
    req->setHeaderMap(headers);
    if (req->getHeader("host").empty())
        req->setHeader("Host", uri->getHost());
    req->setContent(content);
    return DoRequest(req, uri, timeout_ms);
}
// static
HttpResult::ptr HttpClient::DoRequest(HttpRequest::ptr req,
                                Uri::ptr uri,
                                int64_t timeout_ms)
{
    Address::ptr addr = uri->createAddress();
    if (!addr)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_HOST,
            nullptr, "invalid host: " + uri->getHost());
    }
    bool is_ssl = uri->getScheme() == "https";
    Socket::ptr sock = is_ssl ? SSLSocket::createTCP(addr) : Socket::createTCP(addr);
    if (!sock)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::CREATE_SOCKET_ERROR,
            nullptr, "create socket fail: " + addr->toString()
            + ", errno=" + std::to_string(errno)
            + ", errstr=" + std::string(strerror(errno)));
    }
    if (!sock->connect(addr))
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::CONNECT_FAIL,
            nullptr, "connect fail: " + addr->toString());
    }
    sock->setRecvTimeout(timeout_ms);
    HttpClient::ptr conn = std::make_shared<HttpClient>(sock);
    int ret = conn->sendRequest(req);
    if (ret == 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_CLOSE_BY_PEER,
            nullptr, "send request closed by peer: " + addr->toString());
    }
    else if (ret < 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_SOCKET_ERROR,
            nullptr, "send request socket error errno=" + std::to_string(errno)
            + ", errstr=" + std::string(strerror(errno)));
    }
    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::TIMEOUT,
                   nullptr, "recv response timeout: " + addr->toString()
                   + ", timeout_ms:" + std::to_string(timeout_ms));
    }
    return std::make_shared<HttpResult>((int)HttpResult::Error::OK, rsp, "ok");
}

// static
HttpResult::ptr HttpClient::DoGet(const std::string& url, int64_t timeout_ms,
    const std::map<std::string, std::string, CaseInsensitive>& headers,
    const std::string& content)
{
    return DoRequest(HttpMethod::GET, url, timeout_ms, headers, content);
}
// static
HttpResult::ptr HttpClient::DoGet(Uri::ptr uri, int64_t timeout_ms,
    const std::map<std::string, std::string, CaseInsensitive>& headers,
    const std::string& content)
{
    return DoRequest(HttpMethod::GET, uri, timeout_ms, headers, content);
}

// static
HttpResult::ptr HttpClient::DoPost(const std::string& url, int64_t timeout_ms,
    const std::map<std::string, std::string, CaseInsensitive>& headers,
    const std::string& content)
{
    return DoRequest(HttpMethod::POST, url, timeout_ms, headers, content);
}
// static
HttpResult::ptr HttpClient::DoPost(Uri::ptr uri, int64_t timeout_ms,
    const std::map<std::string, std::string, CaseInsensitive>& headers,
    const std::string& content)
{
    return DoRequest(HttpMethod::POST, uri, timeout_ms, headers, content);
}


// global
std::ostream& operator<<(std::ostream& os, const HttpResult::ptr res)
{
    os << res->toString();
    return os;
}

}

}
