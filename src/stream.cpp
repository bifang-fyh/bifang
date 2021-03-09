#include "stream.h"
#include "log.h"

namespace bifang
{

SystemLogger();

int Stream::readFixSize(void* buffer, size_t length)
{
    size_t offset = 0;
    int64_t left = length;
    while (left > 0)
    {
        int64_t len = read((char*)buffer + offset, left);
        if (len <= 0)
        {
            log_error << "read buffer no complete";
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::readFixSize(Buffer::ptr buffer, size_t length)
{
    int64_t left = length;
    while (left > 0)
    {
        int64_t len = read(buffer, left);
        if (len <= 0)
        {
            log_error << "read buffer no complete";
            return len;
        }
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(const void* buffer, size_t length)
{
    size_t offset = 0;
    int64_t left = length;
    while (left > 0)
    {
        int64_t len = write((const char*)buffer + offset, left);
        if (len <= 0)
        {
            log_error << "write buffer no complete";
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(Buffer::ptr buffer, size_t length)
{
    int64_t left = length;
    while (left > 0)
    {
        int64_t len = write(buffer, left);
        if (len <= 0)
        {
            log_error << "write buffer no complete";
            return len;
        }
        left -= len;
    }
    return length;
}

}
