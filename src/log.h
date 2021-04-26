/**
 * brief: 日志模块
 * details:
 * {
 *     "logs" :
 *     [
 *         {
 *             "name" : "root",
 *             "level" : "debug",
 *             "appenders" :
 *             [
 *                 {
 *                     "type" : "stdout",
 *                     "level" : "debug",
 *                     "formatter" : "%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 *                 }
 *             ]
 *         },
 *         {
 *             "name" : "system",
 *             "level" : "debug",
 *             "appenders" :
 *             [
 *                 {
 *                     "type" : "stdout",
 *                     "level" : "debug",
 *                     "formatter" : "%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 *                 },
 *                 {
 *                     "type" : "file",
 *                     "level" : "debug",
 *                     "file" : "logs/system_file_log.txt",
 *                     "formatter" : "%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 *                 },
 *                 {
 *                     "type" : "async",
 *                     "level" : "debug",
 *                     "file" : "logs/system_async_log.txt",
 *                     "interval" : 2000,
 *                     "formatter" : "%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 *                 },
 *                 {
 *                     "type" : "udp",
 *                     "level" : "debug",
 *                     "address" : "127.0.0.1:7777",
 *                     "interval" : 2000,
 *                     "formatter" : "%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 *                 }
 *             ]
 *         }
 *     ]
 * }
 */
#ifndef __BIFANG_LOG_H
#define __BIFANG_LOG_H

#include <string>
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <time.h>

#include "singleton.h"
#include "lock.h"
#include "util.h"
#include "thread.h"
#include "process.h"
#include "fiber.h"
#include "socket.h"


// 获取启动至今的时间差
#define CalculateElapse() \
    bifang::ProcessMgr::GetInstance()->getParentStartTime() == 0 ? 0 : \
        (time(0) - bifang::ProcessMgr::GetInstance()->getParentStartTime())

// 使用流文本的方式输出日志(同步, 异步, udp)
#define log_stream(level) \
    if (level >= logger->getLevel()) \
        bifang::LogEventWrap(bifang::LogEvent::ptr(new bifang::LogEvent(__FILE__, \
            __LINE__, CalculateElapse(), bifang::Thread::getId(), bifang::Fiber::getId(), \
            time(0), bifang::Thread::getName(), logger, level))).getSS()

// 使用流文本的方式输出日志(同步, 异步, udp)
#define log_debug    log_stream(bifang::LogLevel::DEBUG)
#define log_info     log_stream(bifang::LogLevel::INFO)
#define log_warn     log_stream(bifang::LogLevel::WARN)
#define log_error    log_stream(bifang::LogLevel::ERROR)
#define log_fatal    log_stream(bifang::LogLevel::FATAL)


// 使用格式化文本的方式输出日志(同步, 异步, udp)
#define log_fmt(level, fmt, ...) \
    if (level >= logger->getLevel()) \
        bifang::LogEventWrap(bifang::LogEvent::ptr(new bifang::LogEvent(__FILE__, \
            __LINE__, CalculateElapse(), bifang::Thread::getId(), bifang::Fiber::getId(), time(0), \
            bifang::Thread::getName(), logger, level))).getEvent()->format(fmt, __VA_ARGS__)

// 使用格式化文本的方式输出日志(同步, 异步, udp)
#define log_fmt_debug(fmt, ...)    log_fmt(bifang::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define log_fmt_info(fmt,  ...)    log_fmt(bifang::LogLevel::INFO,  fmt, __VA_ARGS__)
#define log_fmt_warn(fmt,  ...)    log_fmt(bifang::LogLevel::WARN,  fmt, __VA_ARGS__)
#define log_fmt_error(fmt, ...)    log_fmt(bifang::LogLevel::ERROR, fmt, __VA_ARGS__)
#define log_fmt_fatal(fmt, ...)    log_fmt(bifang::LogLevel::FATAL, fmt, __VA_ARGS__)


// 获取名称为name的日志器
#define NameLogger(name) static bifang::Logger::ptr logger = bifang::LoggerMgr::GetInstance()->get(name);
// 获取root日志器(应用程序使用)
#define RootLogger() NameLogger("root")
// 获取system日志器(系统函数使用)
#define SystemLogger() NameLogger("system")


namespace bifang
{

class Logger;
class LoggerManager;

/**
 * brief: 日志级别类
 */
class LogLevel
{
public:
    /**
     * brief: 日志级别
     */
    enum Level
    {
        UNKNOW = 0,
        DEBUG  = 1,
        INFO   = 2,
        WARN   = 3,
        ERROR  = 4,
        FATAL  = 5
    };

    /**
     * brief: 将日志级别转成字符串
     * param: 日志级别
     */
    static const std::string toString(LogLevel::Level level)
    {
    #define XX(level) \
        case LogLevel::level: \
            return #level;

        switch (level)
        {
            XX(UNKNOW);
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
            default:
                return "UNKNOW";
        }

    #undef XX
    }

    /**
     * brief: 将字符串转成日志级别
     * param: 日志级别字符串
     */
    static LogLevel::Level toLevel(const std::string &str)
    {
    #define XX(level, s) \
        if (tmp == #s) \
            return LogLevel::level;

        std::string tmp = str;
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

        XX(UNKNOW, UNKNOW);
        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);

        return LogLevel::UNKNOW;
    #undef XX
    }
};

/**
 * brief: 日志事件类
 */
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;

    /**
     * brief: 构造函数
     * param: file 文件名
     *        line 文件行号
     *        elapse 程序启动至今的时间(精确到s)
     *        threadId 线程id
     *        fiberId 协程id
     *        time 日志时间(精确到s)
     *        threadName 线程名称
     *        logger 日志器
     *        level 日志级别
     */
    LogEvent(const char* file, uint32_t line, uint64_t elapse,
            uint32_t threadId, uint32_t fiberId, uint64_t time,
            const std::string &threadName, std::shared_ptr<Logger> logger,
            LogLevel::Level level)
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(threadId)
        ,m_fiberId(fiberId)
        ,m_time(time)
        ,m_threadName(threadName)
        ,m_logger(logger)
        ,m_level(level)
    {
    }

    /**
     * brief: 写入格式化的日志内容
     */
    void format(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, ap);
        if (len != -1)
        {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    const char* getFile()               const { return m_file;       }
    uint32_t getLine()                  const { return m_line;       }
    time_t getElapse()                  const { return m_elapse;     }
    uint32_t getThreadId()              const { return m_threadId;   }
    uint32_t getFiberId()               const { return m_fiberId;    }
    time_t getTime()                    const { return m_time;       }
    const std::string& getThreadName()  const { return m_threadName; }
    std::shared_ptr<Logger> getLogger() const { return m_logger;     }
    LogLevel::Level getLevel()          const { return m_level;      }
    const std::string getContent()      const { return m_ss.str();   }
    std::stringstream& getSS()                { return m_ss;         }

private:
    // 文件名
    const char* m_file;
    // 行号
    uint32_t m_line;
    // 程序启动至今的时间
    time_t m_elapse;
    // 线程id
    uint32_t m_threadId;
    // 协程id
    uint32_t m_fiberId;
    // 时间戳
    time_t m_time;
    // 线程名称
    std::string m_threadName;
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志级别
    LogLevel::Level m_level;
    // 日志内容流
    std::stringstream m_ss;
};

/**
 * brief: 日志格式器类
 */
class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    /**
     * details:
     *     %n 换行
     *     %T 制表符
     *     %d 时间 %d{...}
     *     %t 线程id
     *     %F 协程id
     *     %p 日志级别
     *     %m 日志内容
     *     %r 线程启动至今的秒数
     *     %c 日志器名称
     *     %f 文件名
     *     %l 行号
     *     %N 线程名称
     *
     * 默认格式: %d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
     * 示例: "2020-07-22 23:34:17    5s    10    thread_1    10    [DEBUG]    [root]    root_log:30    thread create error\n"
     */
    LogFormatter(const std::string& pattern);

    /**
     * brief: 返回格式化日志文本流
     * param: os 输出流
     *        level 日志级别
     *        event 日志事件
     */
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event);
    /**
     * brief: 返回格式化日志文本流
     * param: str 输出字符串
     *        level 日志级别
     *        event 日志事件
     */
    void format(std::string& str, LogLevel::Level level, LogEvent::ptr event);

    /**
     * brief: 日志格式是否有错误
     */
    bool isError() const { return m_error; }

    /**
     * brief: 获取日志模板
     */
    const std::string& getPattern() const { return m_pattern; }

private:
    /**
     * brief: 初始化，解析日志模板
     */
    void init();

private:
    // 日志格式模板
    std::string m_pattern;
    // 解析后的日志格式
    std::vector<std::pair<char, std::string> > m_items;
    // 错误标识
    bool m_error = false;
};

/**
 * brief: 日志输出目标基类
 */
class LogAppender
{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;
    
    LogAppender()
    {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%r%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    virtual ~LogAppender() {}

    /**
     * brief: 写入日志(接口)
     * param: level 日志级别
     *        event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    /**
     * brief: 将日志输出目标的配置转成JSON String(接口)
     */
    virtual std::string toJsonString() = 0;

    /**
     * brief: 重新打开文件(file, async专用)
     */
    virtual void reopen() {}

    /**
     * brief: 设置日志格式器
     */
    void setFormatter(LogFormatter::ptr val)
    {
        MutexType::Lock lock(m_mutex);
        m_formatter = val;
    }
    /**
     * brief: 获取日志格式器
     */
    LogFormatter::ptr getFormatter()
    {
        MutexType::Lock lock(m_mutex);
        return m_formatter;
    }

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }

protected:
    // Mutex
    MutexType m_mutex;
    // 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 日志格式器
    LogFormatter::ptr m_formatter;
};

/**
 * brief: 日志器类
 */
class Logger
{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SpinLock MutexType;

    Logger(const std::string& name);

    /**
     * brief: 写日志
     * param: level 日志级别
     *        event 日志事件
     */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /**
     * brief: 添加日志目标
     */
    void add(LogAppender::ptr appender);
    /**
     * brief: 删除日志目标
     */
    bool del(LogAppender::ptr appender);
    /**
     * brief: 清空日志目标
     */
    void clear();

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }

    const std::string& getName() const { return m_name; }

    /**
     * brief: 将日志器的配置转成JSON String
     */
    std::string toJsonString();

    void reopen();

private:
    // Mutex
    MutexType m_mutex;
    // 日志名称
    std::string m_name;
    // 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 日志目标集合
    std::list<LogAppender::ptr> m_appenders;
    // 主日志器
    Logger::ptr m_root;
};

/**
 * brief: 控制台输出类
 */
class StdoutLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    StdoutLogAppender(bool enable_color = true);

    /**
     * brief: 写日志
     * param: level 日志级别
     *        event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    /**
     * brief: 将控制台输出类配置转成JSON String
     */
    virtual std::string toJsonString() override;

private:
    bool m_enable_color;
};

/**
 * brief: 文件输出类
 */
class FileLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    FileLogAppender(const std::string& filename, uint64_t rolling_time,
        const std::string& rolling_dir);

    void rolling();

    /**
     * brief: 写日志
     * param: level 日志级别
     *        event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    /**
     * brief: 将文件输出类配置转成JSON String
     */
    virtual std::string toJsonString() override;

    virtual void reopen() override;

private:
    // 日志输出文件名
    std::string m_filename;
    // 输出文件流
    std::ofstream m_filestream;
    // 日志循环周期
    uint64_t m_rolling_time;
    // 旧日志日志保存根目录
    std::string m_rolling_dir;
    bool m_flag = true;
};

/**
 * brief: 日志缓冲区
 */
class LogBuffer : Noncopyable
{
public:
    typedef std::shared_ptr<LogBuffer> ptr;

    LogBuffer(size_t size = 40960)
        :m_data(new char[size])
        ,m_size(size)
        ,m_cur(0)
    {
    }

    ~LogBuffer()
    {
        delete m_data;
    }

    const char* data() const { return m_data;         }
    size_t size()      const { return m_size;         }
    size_t length()    const { return m_cur;          }
    size_t avail()     const { return m_size - m_cur; }
    void clear()
    {
        memset(m_data, 0, m_size);
        m_cur = 0;
    }
    void write(const std::string& str)
    {
        size_t size = str.size();
        if (avail() > size)
        {
            memcpy(m_data + m_cur, &str[0], size);
            m_cur += size;
        }
    }

private:
    char* m_data;
    size_t m_size;
    size_t m_cur;
};

/**
 * brief: 异步输出类(只有文件, 没有控制台, 控制台做异步没有如何意义)
 */
class AsyncLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<AsyncLogAppender> ptr;

    AsyncLogAppender(const std::string& filename, uint64_t interval,
        uint64_t rolling_time, const std::string& rolling_dir);

    ~AsyncLogAppender();

    void rolling();

    /**
     * brief: 写日志
     * param: level 日志级别
     *        event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    /**
     * brief: 将文件输出类配置转成JSON String
     */
    virtual std::string toJsonString() override;

    virtual void reopen() override;

private:
    /**
     * brief: 线程函数, 用于异步写日志
     */
    void run();

private:
    // 日志输出文件名
    std::string m_filename;
    // 日志输出时间间隔
    uint64_t m_interval;
    // 缓冲区
    std::vector<LogBuffer::ptr> m_buffers;
    // 线程类指针
    Thread::ptr m_thread;
    // // 输出文件流
    std::ofstream m_filestream;
    // 当前异步线程是否运行
    bool m_running = true;
    // 日志循环周期
    uint64_t m_rolling_time;
    // 旧日志日志保存根目录
    std::string m_rolling_dir;
    bool m_flag = true;
};

/**
 * brief: udp输出类
 */
class UDPLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<UDPLogAppender> ptr;

    UDPLogAppender(const std::string& addrstr, uint64_t interval = 2000);

    ~UDPLogAppender();

    /**
     * brief: 重新定位新的udp输出地址
     * param: addrstr 网络地址字符串(host:port)
     */
    bool redirect(const std::string& addrstr);

    /**
     * brief: 写日志
     * param: level 日志级别
     *        event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    /**
     * brief: 将文件输出类配置转成JSON String
     */
    virtual std::string toJsonString() override;

private:
    /**
     * brief: 线程函数, 用于异步写日志
     */
    void run();

private:
    // 网络地址字符串
    std::string m_addrstr;
    // 日志输出时间间隔
    uint64_t m_interval;
    // 缓冲区
    std::vector<LogBuffer::ptr> m_buffers;
    // 线程类指针
    Thread::ptr m_thread;
    // 网络地址指针
    Address::ptr m_addr;
    // socket指针
    Socket::ptr m_socket;
    // 当前异步线程是否运行
    bool m_running = true;
};

/**
 * brief: 日志事件包装类
 */
class LogEventWrap
{
public:
    LogEventWrap(LogEvent::ptr e)
        :m_event(e)
    {
    }

    ~LogEventWrap()
    {
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    /**
     * brief: 获取日志事件
     */
    LogEvent::ptr getEvent() const { return m_event; }

    /**
     * brief: 获取日志内容流
     */
    std::stringstream& getSS() { return m_event->getSS(); }

private:
    // 日志事件
    LogEvent::ptr m_event;
};

/**
 * brief: 日志器管理类
 */
class LoggerManager
{
public:
    typedef SpinLock MutexType;

    LoggerManager();

    /**
     * brief: 获取名字为 name 的日志器
     */
    Logger::ptr get(const std::string& name);

    /**
     * brief: 将所有的日志器配置转成JSON String
     */
    std::string toJsonString();

    void reopen();

private:
    // mutex
    MutexType m_mutex;
    // 日志器列表
    std::unordered_map<std::string, Logger::ptr> m_loggers;
    // 根日志器(所有衍生的日志器如果没有appender, 则使用m_root进行日志输出)
    Logger::ptr m_root;
};

// 日志器管理类单例
typedef bifang::Singleton<LoggerManager> LoggerMgr;

}

#endif /*__BIFANG_LOG_H*/
