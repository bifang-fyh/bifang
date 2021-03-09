#include "Iconv.h"
#include "log.h"

namespace bifang
{

SystemLogger();

Iconv::Iconv(const std::string& tocode, const std::string& fromcode)
    :m_iconv((iconv_t)-1)
{
    m_iconv = iconv_open(tocode.c_str(), fromcode.c_str());
    if (!isVaild())
    {
        log_error << "tocode: " << tocode << ", fromcode: " << fromcode;
        log_error << "errno=" << errno << ", errstr=" << strerror(errno);
        log_error << "please input 'iconv --list' to look up supported format";
    }
}

Iconv::~Iconv()
{
    close();
}

bool Iconv::open(const std::string& tocode, const std::string& fromcode)
{
    close();

    m_iconv = iconv_open(tocode.c_str(), fromcode.c_str());
    if (!isVaild())
    {
        log_error << "tocode: " << tocode << ", fromcode: " << fromcode;
        log_error << "errno=" << errno << ", errstr=" << strerror(errno);
        log_error << "please input 'iconv --list' to look up supported format";
        return false;
    }

    return true;
}

void Iconv::close()
{
    if (isVaild() && iconv_close(m_iconv))
        log_error << "errno=" << errno << ", errstr=" << strerror(errno);

    m_iconv = (iconv_t)-1;
}

bool Iconv::change(const std::string& inbuf, std::string& outbuf)
{
    if (!isVaild())
    {
        log_error << "m_iconv isn't init";
        return false;
    }

    MutexType::Lock lock(m_mutex);
    const char* in = &inbuf[0];
    size_t input_len = inbuf.size();
    size_t output_len = input_len * 1.5;
    outbuf.resize(output_len);
    char* out = &outbuf[0];

    if ((size_t)-1 == iconv(m_iconv, (char**)&in, &input_len, &out, &output_len))
    {
        log_error << "errno=" << errno << ", errstr=" << strerror(errno);
        outbuf.clear();
        return false;
    }
    out = &outbuf[0];
    outbuf.resize(strlen(out));

    return true;
}

// static
bool Iconv::GBKtoUTF8(const std::string& inbuf, std::string& outbuf)
{
    Iconv::ptr ico(new Iconv("UTF-8", "GB18030"));

    return ico->change(inbuf, outbuf);
}

// static
bool Iconv::UTF8toGBK(const std::string& inbuf, std::string& outbuf)
{
    Iconv::ptr ico(new Iconv("GB18030", "UTF-8"));

    return ico->change(inbuf, outbuf);
}


}
