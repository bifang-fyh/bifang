#include "http_session.h"
#include "http_parse.h"
#include "log.h"

namespace bifang
{

namespace http
{

SystemLogger();

HttpSession::HttpSession(Socket::ptr sock)
    :SocketStream(sock)
{
}

HttpRequest::ptr HttpSession::recvRequest()
{
#define XX(str) \
    log_error << str; \
    close(); \
    delete data; \
    return nullptr

    HttpRequestParser::ptr parser(new HttpRequestParser());
    uint64_t buff_size = HttpRequestParser::getHttpRequestBufferSize();
    char* data = new char[buff_size];
    int data_len = 0;
    int parse_offset = 0;
    do
    {
        int len = read(data + data_len, buff_size - data_len);
        if (len <= 0)
        {
            XX("read request data error!");
        }
        data_len += len;

        size_t nparse = parser->execute(data, data_len, parse_offset);
        if (parser->hasError())
        {
            XX("parse request data error!");
        }
        parse_offset += nparse;

        if (parser->isFinished())
            break;

        if (data_len == (int)buff_size) // 超出缓冲区依然没有解析完毕
        {
            XX("http request buffer size is too little!");
        }
    }
    while (1);

    int64_t length = parser->getContentLength();
    if (length > 0)
    {
        std::string& content = parser->getData()->getContent();
        content.resize(length);
        int start = 0;
        int offset = data_len - parse_offset;
        if (offset > 0)
        {
            memcpy(&content[0], data + parse_offset, offset);
            start += offset;
            length -= offset;
        }
        if (length > 0) // 说明socket上面还有数据没有读完
        {
            if (readFixSize(&content[start], length) <= 0)
            {
                XX("read request content data error!");
            }
        }
        parser->getData()->initContent();
    }

    delete data;
    return parser->getData();

#undef XX
}

int64_t HttpSession::sendResponse(HttpResponse::ptr rsp)
{
    static const int block_size = 50 * 1024;
    time_t start_time = time(0);
    size_t rate = rsp->getRate();
    size_t rate_after = rsp->getRateAfter();
    int64_t count = 0;
    std::string data = rsp->toString();
    int64_t size = data.size();
    while (count < size)
    {
        if (rate)
        {
            int64_t limit = (int64_t)rate * (time(0) - start_time + 1) - (count / 1024 - rate_after);
            if (limit <= 0)
            {
                size_t delay = (size_t)(-limit * 1000 / rate + 1);
                usleep(delay * 1000);
            }
        }

        int cnt = writeFixSize(data.c_str() + count, size - count >= block_size ? block_size : size - count);
        if (cnt <= 0)
            return cnt;
        count += cnt;
    }

    std::string filename = rsp->getFilename();
    int64_t start = rsp->getStart();
    int64_t len = rsp->getLength();
    if (!filename.empty())
    {
        std::ifstream ifs(filename);
        ifs.seekg(start, std::ios::beg);
        char* __data = new char[block_size];

        while (len > 0)
        {
            if (rate)
            {
                int64_t limit = (int64_t)rate * (time(0) - start_time + 1) - (count / 1024 - rate_after);
                if (limit <= 0)
                {
                    size_t delay = (size_t)(-limit * 1000 / rate + 1);
                    usleep(delay * 1000);
                }
            }

            if (len > block_size)
                ifs.read(__data, block_size);
            else
                ifs.read(__data, len);

            int cnt = writeFixSize(__data, ifs.gcount());
            if (cnt <= 0)
            {
                delete __data;
                return cnt;
            }

            count += cnt;
            len -= ifs.gcount();
        }
        delete __data;
    }

    return count;
}

}

}
