#ifndef __BIFANG_URI_H
#define __BIFANG_URI_H

#include <memory>
#include <string>
#include <sstream>
#include <algorithm>

#include "address.h"

namespace bifang
{

/*
     foo://user@example.com:8042/over/there?name=ferret#nose
     \_/   \___________________/\_________/ \_________/ \__/
      |              |              |            |        |
   scheme        authority         path        query   fragment
      |   _____________________|__
     / \ /                        \
     urn:example:animal:ferret:nose
*/

/**
 * brief: URI类
 */
class Uri
{
public:
    typedef std::shared_ptr<Uri> ptr;

    Uri():m_port(0)
    {
    }

    /**
     * brief: 创建Uri对象
     * param: uri uri字符串
     * return 解析成功返回Uri对象否则返回nullptr
     */
    static Uri::ptr create(const std::string& uristr);

    /**
     * brief: 获取Address
     */
    Address::ptr createAddress() const;

    /**
     * brief: 是否默认端口
     */
    bool isDefaultPort() const;

public:
    const std::string& getScheme()   const { return m_scheme;   }
    const std::string& getUserinfo() const { return m_userinfo; }
    const std::string& getHost()     const { return m_host;     }
    const std::string& getPath() const
    {
        static const std::string default_value = "/";
        return m_path.empty() ? default_value : m_path;
    }
    const std::string& getQuery()    const { return m_query;    }
    const std::string& getFragment() const { return m_fragment; }
    int32_t getPort() const
    {
        if (m_port)
            return m_port;
        else if (m_scheme == "http" || m_scheme == "ws")
            return 80;
        else if(m_scheme == "https" || m_scheme == "wss")
            return 443;
        return m_port;
    }

    void setScheme(const std::string& v)   { m_scheme = v;   }
    void setUserinfo(const std::string& v) { m_userinfo = v; }
    void setHost(const std::string& v)     { m_host = v;     }
    void setPath(const std::string& v)     { m_path = v;     }
    void setQuery(const std::string& v)    { m_query = v;    }
    void setFragment(const std::string& v) { m_fragment = v; }
    void setPort(int32_t v)                { m_port = v;     }

    std::string toString() const;

private:
    // scheme
    std::string m_scheme;
    // 用户信息
    std::string m_userinfo;
    // host
    std::string m_host;
    // 路径
    std::string m_path;
    // 查询参数
    std::string m_query;
    // fragment
    std::string m_fragment;
    // 端口
    int32_t m_port;
};

}

#endif /*__BIFANG_URI_H*/
