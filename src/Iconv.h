/**
 * brief: iconv封装
 */
#ifndef __BIFANG_ICONV_H
#define __BIFANG_ICONV_H

#include <memory>
#include <string>
#include <iconv.h>
#include <string.h>

#include "noncopyable.h"
#include "lock.h"

namespace bifang
{

class Iconv : Noncopyable
{
public:
    typedef std::shared_ptr<Iconv> ptr;
    typedef Mutex MutexType;

    Iconv(const std::string& tocode, const std::string& fromcode);

    ~Iconv();

    /**
     * brief: 生成新的iconv_t(若m_iconv已存在, 则先关闭原先的再生成新的)
     * param: tocode 目标编码
     *        fromcode 源编码
     */
    bool open(const std::string& tocode, const std::string& fromcode);

    /**
     * brief: 关闭当前iconv_t
     */
    void close();

    bool change(const std::string& inbuf, std::string& outbuf);

    /**
     * brief: 当前是否有效
     */
    inline bool isVaild() const { return m_iconv != (iconv_t)-1; }

public:
    /**
     * brief: GBK转UTF8格式(实际使用了GB18030, 因为其兼容GBK和一些旧版本的中文编码)
     */
    static bool GBKtoUTF8(const std::string& inbuf, std::string& outbuf);

    /**
     * brief: UTF8转GBK格式(实际使用了GB18030, 因为其兼容GBK和一些旧版本的中文编码)
     */
    static bool UTF8toGBK(const std::string& inbuf, std::string& outbuf);

private:
    // mutex
    MutexType m_mutex;
    // 编码转换描述符
    iconv_t m_iconv;
};

}


#endif /*__BIFANG_ICONV_H*/
