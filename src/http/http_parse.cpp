#include "http_parse.h"
#include "log.h"
#include "config.h"

namespace bifang
{

namespace http
{

SystemLogger();

static Config<uint64_t>::ptr g_http_request_buffer_size =
           ConfigMgr::GetInstance()->get("http.request.buffer_size",
               (uint64_t)(256 * 1024), "http request buffer size");

static Config<uint64_t>::ptr g_http_response_buffer_size =
           ConfigMgr::GetInstance()->get("http.response.buffer_size",
               (uint64_t)(256 * 1024), "http response buffer size");

static uint64_t s_http_request_buffer_size = 0;
static uint64_t s_http_response_buffer_size = 0;

namespace
{
struct ParseSizeIniter
{
    ParseSizeIniter()
    {
        s_http_request_buffer_size = g_http_request_buffer_size->getValue();
        s_http_response_buffer_size = g_http_response_buffer_size->getValue();

        g_http_request_buffer_size->addCallback(
                [](const uint64_t& ov, const uint64_t& nv)
                {
                    s_http_request_buffer_size = nv;
                });

        g_http_response_buffer_size->addCallback(
                [](const uint64_t& ov, const uint64_t& nv)
                {
                    s_http_response_buffer_size = nv;
                });
    }
};

static ParseSizeIniter s_init;
}

static void on_request_method(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    HttpMethod m = CharsToHttpMethod(at);

    if (m == HttpMethod::INVALID_METHOD)
    {
        log_warn << "invalid http request method: " << std::string(at, length);
        parser->setError(1000);
        return;
    }
    parser->getData()->setMethod(m);
}

static void on_request_fragment(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setFragment(std::string(at, length));
}

static void on_request_path(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setPath(std::string(at, length));
}

static void on_request_query(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getData()->setQuery(std::string(at, length));
}

static void on_request_version(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    uint8_t v = 0;
    if (strncasecmp(at, "HTTP/1.1", length) == 0)
        v = 0x11;
    else if (strncasecmp(at, "HTTP/1.0", length) == 0)
        v = 0x10;
    else
    {
        log_warn << "invalid http request version: " << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getData()->setVersion(v);
}

static void on_request_http_field(void *data, const char *field, size_t flen,
    const char *value, size_t vlen)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if (flen == 0)
    {
        log_warn << "invalid http request field length == 0";
        return;
    }
    parser->getData()->setHeader(std::string(field, flen),
                           std::string(value, vlen));
}

HttpRequestParser::HttpRequestParser()
{
    m_data.reset(new bifang::http::HttpRequest(true));
    http_server_parser_init(&m_parser);
    m_parser.request_method = on_request_method;
    m_parser.request_uri = NULL;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = NULL;
    m_parser.http_field = on_request_http_field;
    m_parser.data = this;
}

size_t HttpRequestParser::execute(char* data, size_t len, size_t off)
{
    size_t offset = http_server_parser_execute(&m_parser, data, len, off);
    return offset;
}

// static
uint64_t HttpRequestParser::getHttpRequestBufferSize()
{
    return s_http_request_buffer_size;
}


static void on_response_reason(void *data, const char *at, size_t length)
{
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    parser->getData()->setReason(std::string(at, length));
}

static void on_response_status(void *data, const char *at, size_t length)
{
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    bifang::http::HttpStatus status = (bifang::http::HttpStatus)(atoi(at));
    parser->getData()->setStatus(status);
}

static void on_response_version(void *data, const char *at, size_t length)
{
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    uint8_t v = 0;
    if (strncasecmp(at, "HTTP/1.1", length) == 0)
        v = 0x11;
    else if (strncasecmp(at, "HTTP/1.0", length) == 0)
        v = 0x10;
    else
    {
        log_warn << "invalid http response version: " << std::string(at, length);
        parser->setError(1001);
        return;
    }

    parser->getData()->setVersion(v);
}

static void on_response_http_field(void *data, const char *field, size_t flen,
    const char *value, size_t vlen)
{
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    if (flen == 0)
    {
        log_warn << "invalid http response field length == 0";
        return;
    }
    parser->getData()->setHeader(std::string(field, flen),
        std::string(value, vlen));
}


HttpResponseParser::HttpResponseParser()
{
    m_data.reset(new HttpResponse(true));
    http_client_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = NULL;
    m_parser.http_version = on_response_version;
    m_parser.header_done = NULL;
    m_parser.last_chunk = NULL;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}

int HttpResponseParser::execute(char* data, size_t len, size_t off, bool chunck)
{
    if (chunck)
        http_client_parser_init(&m_parser);

    int offset = http_client_parser_execute(&m_parser, data, len, off);
    return offset;
}

// static
uint64_t HttpResponseParser::getHttpResponseBufferSize()
{
    return s_http_response_buffer_size;
}


}

}
