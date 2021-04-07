/**
 * brief: 配置文件(使用json作为配置文件, 可以自行实现序列反序列函数, 以支持其他格式的配置文件)
 */
#ifndef __BIFANG_CONFIG_H
#define __BIFANG_CONFIG_H

#include <string>
#include <vector>
#include <forward_list>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "lock.h"
#include "log.h"
#include "util.h"
#include "singleton.h"
#include "json/json.h"

namespace bifang
{

/**
 * brief: 类型转换模板类(F 源类型, T 目标类型)
 */
template<class F, class T>
struct LexicalCast
{
    /**
     * brief: 类型转换
     * param: v 源类型值
     * return: 返回v转换后的目标类型
     * exception: 当类型不可转换时抛出异常
     */
    T operator()(const F& v)
    {
        return boost::lexical_cast<T>(v);
    }
};

#define SEQUENCE(root, v, message) \
    Json::Value root; \
    Json::String errs; \
    Json::CharReaderBuilder reader; \
    std::unique_ptr<Json::CharReader> const jsonReader(reader.newCharReader()); \
    if (!jsonReader->parse(v.c_str(), v.c_str() + v.length(), &root, &errs)) \
    { \
        std::stringstream ss; \
        ss << message << " strerror: " << errs; \
        throw std::logic_error(ss.str()); \
    }

#define RESEQUENCE(root) \
    std::stringstream ss; \
    ss << root; \
    return ss.str()

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::vector<T>)
 */
template<class T>
struct LexicalCast<std::string, std::vector<T> >
{
    std::vector<T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(vector) is illegal!");

        typename std::vector<T> vec;
        std::stringstream ss;
        for (uint32_t i = 0; i < root.size(); i++)
        {
            ss.str("");
            ss << root[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::vector<T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::vector<T>, std::string>
{
    std::string operator()(const std::vector<T>& v)
    {
        Json::Value root(Json::arrayValue);
        for (auto& i : v)
            root.append(Json::Value(LexicalCast<T, std::string>()(i)));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::forward_list<T>)
 */
template<class T>
struct LexicalCast<std::string, std::forward_list<T> >
{
    std::forward_list<T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(forward_list) is illegal!");

        typename std::forward_list<T> f_list;
        std::stringstream ss;
        for (uint32_t i = root.size(); i > 0; i--)
        {
            ss.str("");
            ss << root[i - 1];
            f_list.push_front(LexicalCast<std::string, T>()(ss.str()));
        }

        return f_list;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::forward_list<T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::forward_list<T>, std::string>
{
    std::string operator()(const std::forward_list<T>& v)
    {
        Json::Value root(Json::arrayValue);
        for (auto& i : v)
            root.append(Json::Value(LexicalCast<T, std::string>()(i)));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::list<T>)
 */
template<class T>
struct LexicalCast<std::string, std::list<T> >
{
    std::list<T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(list) is illegal!");

        typename std::list<T> _list;
        std::stringstream ss;
        for (uint32_t i = 0; i < root.size(); i++)
        {
            ss.str("");
            ss << root[i];
            _list.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return _list;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::list<T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::list<T>, std::string>
{
    std::string operator()(const std::list<T>& v)
    {
        Json::Value root(Json::arrayValue);
        for (auto& i : v)
            root.append(Json::Value(LexicalCast<T, std::string>()(i)));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::set<T>)
 */
template<class T>
struct LexicalCast<std::string, std::set<T> >
{
    std::set<T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(set) is illegal!");

        typename std::set<T> _set;
        std::stringstream ss;
        for (uint32_t i = 0; i < root.size(); i++)
        {
            ss.str("");
            ss << root[i];
            _set.insert(LexicalCast<std::string, T>()(ss.str()));
        }

        return _set;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::set<T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::set<T>, std::string>
{
    std::string operator()(const std::set<T>& v)
    {
        Json::Value root(Json::arrayValue);
        for (auto& i : v)
            root.append(Json::Value(LexicalCast<T, std::string>()(i)));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::unordered_set<T>)
 */
template<class T>
struct LexicalCast<std::string, std::unordered_set<T> >
{
    std::unordered_set<T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(unordered_set) is illegal!");

        typename std::unordered_set<T> u_set;
        std::stringstream ss;
        for (uint32_t i = 0; i < root.size(); i++)
        {
            ss.str("");
            ss << root[i];
            u_set.insert(LexicalCast<std::string, T>()(ss.str()));
        }

        return u_set;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::unordered_set<T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::unordered_set<T>, std::string>
{
    std::string operator()(const std::unordered_set<T>& v)
    {
        Json::Value root(Json::arrayValue);
        for (auto& i : v)
            root.append(Json::Value(LexicalCast<T, std::string>()(i)));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::map<std::string, T>)
 */
template<class T>
struct LexicalCast<std::string, std::map<std::string, T> >
{
    std::map<std::string, T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(map) is illegal!");

        typename std::map<std::string, T> _map;
        std::stringstream ss;
        auto names = root.getMemberNames();
        for (auto& i : names)
        {
            ss.str("");
            ss << root[i];
            _map[i] = LexicalCast<std::string, T>()(ss.str());
        }

        return _map;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::map<std::string, T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::map<std::string, T>, std::string>
{
    std::string operator()(const std::map<std::string, T>& v)
    {
        Json::Value root(Json::objectValue);
 
        for (auto& i : v)
            root[i.first] = Json::Value(LexicalCast<T, std::string>()(i.second));

        RESEQUENCE(root);
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 std::unordered_map<std::string, T>)
 */
template<class T>
struct LexicalCast<std::string, std::unordered_map<std::string, T> >
{
    std::unordered_map<std::string, T> operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(unordered_map) is illegal!");

        typename std::unordered_map<std::string, T> u_map;
        std::stringstream ss;
        auto names = root.getMemberNames();
        for (auto& i : names)
        {
            ss.str("");
            ss << root[i];
            u_map[i] = LexicalCast<std::string, T>()(ss.str());
        }

        return u_map;
    }
};
/**
 * brief: 类型转换模板类偏特化版本(std::unordered_map<std::string, T> 转换成 JSON String)
 */
template<class T>
struct LexicalCast<std::unordered_map<std::string, T>, std::string>
{
    std::string operator()(const std::unordered_map<std::string, T>& v)
    {
        Json::Value root(Json::objectValue);

        for (auto& i : v)
            root[i.first] = Json::Value(LexicalCast<T, std::string>()(i.second));

        RESEQUENCE(root);
    }
};

/**
 * brief: 日志配置相关内容(对应LogAppender类)
 */
struct LogAppenderDefine
{
    int type = 1; // 1 stdout, 2 file, 3 async, 4 udp
    uint64_t interval = 30; // async, udp(output interval)
    std::string address;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const
    {
        return type == oth.type
            && interval == oth.interval
            && address == oth.address
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};
/**
 * brief: 日志器相关内容(对应Logger类)
 */
struct LogDefine
{
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const
    {
        return name == oth.name
            && level == oth.level
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const
    {
        return name < oth.name;
    }
};

/**
 * brief: 类型转换模板类偏特化版本(JSON String 转换成 LogDefine)
 */
template<>
struct LexicalCast<std::string, LogDefine>
{
    LogDefine operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(LogDefine) is illegal!");

        LogDefine ld;
        if (!root.isMember("name"))
        {
            std::cout << "log config error: name is null: " << root << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = root["name"].asString();
        if (root.isMember("level"))
            ld.level = LogLevel::toLevel(root["level"].asString());

        if (root.isMember("appenders"))
        {
            for (uint32_t i = 0; i < root["appenders"].size(); i++)
            {
                auto appender = root["appenders"][i];
                if (!appender.isMember("type"))
                {
                    std::cout << "log config errorlog config error: appender type is null: "
                        << appender << std::endl;
                    continue;
                }
                std::string type = appender["type"].asString();
                std::transform(type.begin(), type.end(), type.begin(), ::tolower);

                LogAppenderDefine lad;
                if (type == "stdout")
                    lad.type = 1;
                else if (type == "file")
                {
                    lad.type = 2;
                    if (!appender.isMember("file"))
                    {
                        std::cout << "log config error: file is null(file): "
                            << appender << std::endl;
                        continue;
                    }
                    lad.file = appender["file"].asString();
                }
                else if (type == "async")
                {
                    lad.type = 3;
                    if (!appender.isMember("file"))
                    {
                        std::cout << "log config error: file is null(async): "
                            << appender << std::endl;
                        continue;
                    }
                    lad.file = appender["file"].asString();
                    if (appender.isMember("interval"))
                        lad.interval = appender["interval"].asUInt64();
                }
                else if (type == "udp")
                {
                    lad.type = 4;
                    if (!appender.isMember("address"))
                    {
                        std::cout << "log config error: output address is null(udp): "
                            << appender << std::endl;
                        continue;
                    }
                    lad.address = appender["address"].asString();
                    if (appender.isMember("interval"))
                        lad.interval = appender["interval"].asUInt64();
                }
                else
                {
                    std::cout << "log config error: appender type is invalid: "
                        << appender << std::endl;
                    continue;
                }

                if (appender.isMember("level"))
                    lad.level = LogLevel::toLevel(appender["level"].asString());

                if (!appender.isMember("formatter"))
                {
                    std::cout << "log config error: appender formatter is null: "
                        << appender << std::endl;
                }
                else
                    lad.formatter = appender["formatter"].asString();

                ld.appenders.push_back(lad);
            }
        }
        return ld;
    }
};

/**
 * brief: 类型转换模板类偏特化版本(LogDefine 转换成 JSON String)
 */
template<>
struct LexicalCast<LogDefine, std::string>
{
    std::string operator()(const LogDefine& ld)
    {
        Json::Value root;

        root["name"] = ld.name;
        root["level"] = LogLevel::toString(ld.level);

        for (auto& a : ld.appenders)
        {
            Json::Value n;

            switch (a.type)
            {
                case 1:
                    n["type"] = "stdout";
                    break;
                case 2:
                    n["type"] = "file";
                    n["file"] = a.file;
                    break;
                case 3:
                    n["type"] = "async";
                    n["file"] = a.file;
                    n["interval"] = a.interval;
                    break;
                case 4:
                    n["type"] = "udp";
                    n["address"] = a.address;
                    n["interval"] = a.interval;
                    break;
                default:
                    break;
            }
            n["level"] = LogLevel::toString(a.level);
            n["formatter"] = a.formatter;

            root["appenders"].append(n);
        }

        RESEQUENCE(root);
    }
};

/**
 * brief: TCP服务配置类
 */
struct TcpServerConf
{
    typedef std::shared_ptr<TcpServerConf> ptr;

    // 服务器类型 http, websocket
    std::string type = "http";
    std::string name;
    int ssl = 0;
    int keepalive = 0;
    int64_t timeout = 2 * 60 * 1000;
    std::vector<std::string> address;
    int gzip = 0;
    uint64_t gzip_min_length = 1024;
    int gzip_comp_level = -1;
    std::string cert_file;
    std::string key_file;
    std::string accept_worker;
    std::string process_worker;

    bool operator==(const TcpServerConf& oth) const
    {
        return type == oth.type
            && name == oth.name
            && ssl == oth.ssl
            && keepalive == oth.keepalive
            && timeout == oth.timeout
            && address == oth.address
            && gzip == oth.gzip
            && gzip_min_length == oth.gzip_min_length
            && gzip_comp_level == oth.gzip_comp_level
            && cert_file == oth.cert_file
            && key_file == oth.key_file
            && accept_worker == oth.accept_worker
            && process_worker == oth.process_worker;
    }
};

template<>
struct LexicalCast<std::string, TcpServerConf>
{
    TcpServerConf operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(TcpServerConf) is illegal!");

        TcpServerConf conf;
        if (root.isMember("type"))
            conf.type = root["type"].asString();

        if (root.isMember("name"))
            conf.name = root["name"].asString();

        if (root.isMember("ssl"))
            conf.ssl = root["ssl"].asInt();

        if (root.isMember("keepalive"))
            conf.keepalive = root["keepalive"].asInt();

        if (root.isMember("timeout"))
            conf.timeout = root["timeout"].asInt64();

        if (root.isMember("address"))
        {
            for (uint32_t i = 0; i < root["address"].size(); i++)
                conf.address.push_back(root["address"][i].asString());
        }

        if (root.isMember("gzip"))
            conf.gzip = root["gzip"].asInt();

        if (root.isMember("gzip_min_length"))
            conf.gzip_min_length = root["gzip_min_length"].asUInt64();

        if (root.isMember("gzip_comp_level"))
            conf.gzip_comp_level = root["gzip_comp_level"].asInt();

        if (root.isMember("cert_file"))
            conf.cert_file = root["cert_file"].asString();

        if (root.isMember("key_file"))
            conf.key_file = root["key_file"].asString();

        if (root.isMember("accept_worker"))
            conf.accept_worker = root["accept_worker"].asString();

        if (root.isMember("process_worker"))
            conf.process_worker = root["process_worker"].asString();

        return conf;
    }
};

template<>
struct LexicalCast<TcpServerConf, std::string>
{
    std::string operator()(const TcpServerConf& conf)
    {
        Json::Value root;

        root["type"] = conf.type;
        root["name"] = conf.name;
        root["ssl"] = conf.ssl;
        root["keepalive"] = conf.keepalive;
        root["timeout"] = conf.timeout;
        for (auto& i : conf.address) 
            root["address"].append(Json::Value(i));
        root["gzip"] = conf.gzip;
        root["gzip_min_length"] = conf.gzip_min_length;
        root["gzip_comp_level"] = conf.gzip_comp_level;
        root["cert_file"] = conf.cert_file;
        root["key_file"] = conf.key_file;
        root["accept_worker"] = conf.accept_worker;
        root["process_worker"] = conf.process_worker;

        RESEQUENCE(root);
    }
};

/**
 * brief: MySql服务配置类
 */
struct MySqlConf
{
    typedef std::shared_ptr<MySqlConf> ptr;

    std::string host;
    int port;
    std::string user;
    std::string passwd;
    std::string dbname;
    uint32_t poolSize = 10;

    bool operator==(const MySqlConf& oth) const
    {
        return host == oth.host
            && port == oth.port
            && user == oth.user
            && passwd == oth.passwd
            && dbname == oth.dbname
            && poolSize == oth.poolSize;
    }
};

template<>
struct LexicalCast<std::string, MySqlConf>
{
    MySqlConf operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(MySqlConf) is illegal!");

        MySqlConf conf;
        if (root.isMember("host"))
            conf.host = root["host"].asString();

        if (root.isMember("port"))
            conf.port = root["port"].asInt();

        if (root.isMember("user"))
            conf.user = root["user"].asString();

        if (root.isMember("passwd"))
            conf.passwd = root["passwd"].asString();

        if (root.isMember("dbname"))
            conf.dbname = root["dbname"].asString();

        if (root.isMember("poolSize"))
            conf.poolSize = root["poolSize"].asInt();

        return conf;
    }
};

template<>
struct LexicalCast<MySqlConf, std::string>
{
    std::string operator()(const MySqlConf& conf)
    {
        Json::Value root;

        root["host"] = conf.host;
        root["port"] = conf.port;
        root["user"] = conf.user;
        root["passwd"] = conf.passwd;
        root["dbname"] = conf.dbname;
        root["poolSize"] = conf.poolSize;

        RESEQUENCE(root);
    }
};

/**
 * brief: Redis服务配置类
 */
struct RedisConf
{
    typedef std::shared_ptr<RedisConf> ptr;

    std::string host;
    int port;
    std::string passwd;
    uint32_t connect_timeout = 100;
    uint32_t cmd_timeout = 100;
    uint32_t poolSize = 10;

    bool operator==(const RedisConf& oth) const
    {
        return host == oth.host
            && port == oth.port
            && passwd == oth.passwd
            && connect_timeout == oth.connect_timeout
            && cmd_timeout == oth.cmd_timeout
            && poolSize == oth.poolSize;
    }
};

template<>
struct LexicalCast<std::string, RedisConf>
{
    RedisConf operator()(const std::string& v)
    {
        SEQUENCE(root, v, "json string(RedisConf) is illegal!");

        RedisConf conf;
        if (root.isMember("host"))
            conf.host = root["host"].asString();

        if (root.isMember("port"))
            conf.port = root["port"].asInt();

        if (root.isMember("passwd"))
            conf.passwd = root["passwd"].asString();

        if (root.isMember("connect_timeout"))
            conf.connect_timeout = root["connect_timeout"].asInt();

        if (root.isMember("cmd_timeout"))
            conf.cmd_timeout = root["cmd_timeout"].asInt();

        if (root.isMember("poolSize"))
            conf.poolSize = root["poolSize"].asInt();

        return conf;
    }
};

template<>
struct LexicalCast<RedisConf, std::string>
{
    std::string operator()(const RedisConf& conf)
    {
        Json::Value root;
        root["host"] = conf.host;
        root["port"] = conf.port;
        root["passwd"] = conf.passwd;
        root["connect_timeout"] = conf.connect_timeout;
        root["cmd_timeout"] = conf.cmd_timeout;
        root["poolSize"] = conf.poolSize;

        RESEQUENCE(root);
    }
};

/**
 * brief: 配置基类
 */
class ConfigBase
{
public:
    typedef std::shared_ptr<ConfigBase> ptr;

    /**
     * brief: 构造函数
     * param: name 参数名称(自动转换为小写, 故参数名称不区分大小写)
     *        description 参数描述
     */
    ConfigBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description)
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    virtual ~ConfigBase() {}

    /**
     * brief: 返回配置参数名称
     */
    const std::string& getName() const { return m_name; }

    /**
     * brief: 返回配置参数的描述
     */
    const std::string& getDescription() const { return m_description; }

    /**
     * brief: 返回配置参数值的类型名称
     */
    virtual std::string getTypeName() const = 0;

    /**
     * brief: 将配置参数转成字符串
     */
    virtual std::string toString() = 0;

    /**
     * brief: 从字符串初始化值
     */
    virtual bool fromString(const std::string& val) = 0;

protected:
    // 参数名称
    std::string m_name;
    // 参数描述
    std::string m_description;
};

/**
 * brief: 配置参数子类, 保存对应类型的参数值
 * details: T 参数的具体类型
 *          FromStr 从std::string转换成T类型的仿函数
 *          ToStr 从T类型转换成std::string的仿函数
 *          std::string 为JSON格式的字符串
 */
template<class T, class FromStr = LexicalCast<std::string, T>,
                  class ToStr = LexicalCast<T, std::string> >
class Config : public ConfigBase
{
public:
    typedef std::shared_ptr<Config> ptr;
    typedef RWMutex RWMutexType;
    typedef std::function<void(const T& old_value, const T& new_value)> OnChangeCb;

    /**
     * brief: 通过参数名, 参数值, 描述构造Config
     * param: name 参数名称
     *        default_value 参数默认值
     *        description 参数描述
     */
    Config(const std::string& name,
           const T& default_value,
           const std::string& description = "")
        :ConfigBase(name, description)
        ,m_val(default_value)
    {
    }

    /**
     * brief: 将参数值转换成 JSON String
     */
    std::string toString() override
    {
        try
        {
            RWMutexType::ReadLock lock(m_mutex);
            return ToStr()(m_val);
        }
        catch (std::exception& e)
        {
            SystemLogger();
            log_error << "Config::toString exception " << e.what()
                << ", convert: " << type_to_name<T>() << " to string"
                << ", name=" << m_name;
        }
        return "";
    }

    /**
     * brief: 从 JSON String 转成参数的值
     */
    bool fromString(const std::string& val) override
    {
        try
        {
            setValue(FromStr()(val));
        }
        catch (std::exception& e)
        {
            SystemLogger();
            log_error << "Config::fromString exception " << e.what()
                << ", convert: string to " << type_to_name<T>()
                << ", name=" << m_name << ": " << val;
        }
        return false;
    }

    /**
     * brief: 获取当前参数的值
     */
    const T getValue()
    {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }

    /**
     * brief: 设置当前参数的值
     * details: 如果参数的值有发生变化, 则通知对应的注册回调函数
     */
    void setValue(const T& v)
    {
        {
            RWMutexType::ReadLock lock(m_mutex);
            if (v == m_val)
                return;
            for (auto& i : m_cbs)
                i(m_val, v);
        }
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }

    /**
     * brief: 返回参数值的类型名称(typeinfo)
     */
    std::string getTypeName() const override { return type_to_name<T>(); }

public:
    /**
     * brief: 添加变化回调函数
     */
    void addCallback(OnChangeCb cb)
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.push_back(cb);
    }
    /**
     * brief: 清理所有的回调函数
     */
    void clearCallback()
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }

private:
    // RWmutex
    RWMutexType m_mutex;
    // 参数值
    T m_val;
    // 变更回调函数组
    std::vector<OnChangeCb> m_cbs;
};

/**
 * brief: Config管理类
 */
class ConfigManager
{
public:
    typedef RWMutex RWMutexType;
    typedef std::unordered_map<std::string, ConfigBase::ptr> ConfigMap;

    /**
     * brief: 获取对应参数名的配置参数(若不存在则自动创建, 并用default_value赋值)
     * param: name 配置参数名称
     *        default_value 参数默认值
     *        description 参数描述
     * return: 返回对应的配置参数, 如果参数名存在但是类型不匹配则返回nullptr
     */
    template<class T>
    typename Config<T>::ptr get(const std::string& name,
                                const T& default_value,
                                const std::string& description = "")
    {
        RWMutexType::WriteLock lock(m_mutex);
        auto it = m_datas.find(name);
        if (it != m_datas.end())
        {
            SystemLogger();
            auto tmp = std::dynamic_pointer_cast<Config<T> >(it->second);
            if (tmp)
            {
                log_debug << "Lookup name=" << name << " exists";
                return tmp;
            }
            else
            {
                log_debug << "Lookup name=" << name << " exists but type not "
                    << type_to_name<T>() << ", real_type="
                    << it->second->getTypeName() << " " << it->second->toString();
                return nullptr;
            }
        }

        typename Config<T>::ptr v(new Config<T>(name, default_value, description));
        m_datas[v->getName()] = v;
        return v;
    }
    /**
     * brief: 查找配置参数
     * param: name 配置参数名称
     * return: 返回配置参数名为name的配置参数
     */
    template<class T>
    typename Config<T>::ptr get(const std::string& name)
    {
        auto it = m_datas.find(name);
        return it == m_datas.end() ? nullptr : std::dynamic_pointer_cast<Config<T> >(it->second);
    }

    /**
     * brief: 遍历配置模块里面所有配置项
     * param: cb 配置项回调函数
     */
    void visit(std::function<void(ConfigBase::ptr)> cb)
    {
        RWMutexType::ReadLock lock(m_mutex);
        for (auto& it : m_datas)
            cb(it.second);
    }

    /**
     * brief: 从Json::Value中加载配置模块
     */
    void load(Json::Value& root);
    
    /**
     * brief: 读取path文件夹里面的配置文件用以加载配置模块
     */
    void load(const std::string& path);

private:
    // RWmutex
    RWMutexType m_mutex;
    // Config列表
    ConfigMap m_datas;
};

typedef Singleton<ConfigManager> ConfigMgr;

}

#endif /*__BIFANG_CONFIG_H*/
