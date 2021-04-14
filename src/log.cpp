#include <sstream>
#include <unordered_set>

#include "log.h"
#include "config.h"
#include "hook.h"
#include "iomanager.h"
#include "Assert.h"

namespace bifang
{


LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern)
{
    init();
}

void LogFormatter::format(std::ostream& os, LogLevel::Level level,
                       LogEvent::ptr event)
{
    for (auto& i : m_items)
    {
        switch (i.first)
        {
            case 'n': // 换行
            {
                os << std::endl;
                break;
            }
            case 'T': // 制表符
            {
                os << '\t';
                break;
            }
            case 'd': // 时间
            {
                std::string m_fmt;
                if (i.second.empty())
                    m_fmt = "%Y-%m-%d %H:%M:%S";
                else
                    m_fmt = i.second;
                struct tm tm;
                time_t time = event->getTime();
                localtime_r(&time, &tm);
                char buf[64];
                strftime(buf, sizeof(buf), m_fmt.c_str(), &tm);
                os << buf;
                break;
            }
            case 't': // 线程id
            {
                os << event->getThreadId();
                break;
            }
            case 'F': // 协程id
            {
                os << event->getFiberId();
                break;
            }
            case 'p': // 日志级别
            {
                os << LogLevel::toString(level);
                break;
            }
            case 'm': // 日志内容
            {
                os << event->getContent();
                break;
            }
            case 'r': // 线程启动至今的秒数
            {
                os << bifang::time_difference(event->getElapse());
                break;
            }
            case 'c': // 日志名称
            {
                os << event->getLogger()->getName();
                break;
            }
            case 'f': // 文件名
            {
                os << event->getFile();
                break;
            }
            case 'l': // 行号
            {
                os << event->getLine();
                break;
            }
            case 'N': // 线程名称
            {
                os << event->getThreadName();
                break;
            }
            default:
            {
                os << i.second;
                break;
            }
        }
    }
}

void LogFormatter::format(std::string& str, LogLevel::Level level,
                       LogEvent::ptr event)
{
    for (auto& i : m_items)
    {
        switch (i.first)
        {
            case 'n': // 换行
            {
                str.append(1, '\n');
                break;
            }
            case 'T': // 制表符
            {
                str.append(1, '\t');
                break;
            }
            case 'd': // 时间
            {
                std::string m_fmt;
                if (i.second.empty())
                    m_fmt = "%Y-%m-%d %H:%M:%S";
                else
                    m_fmt = i.second;
                struct tm tm;
                time_t time = event->getTime();
                localtime_r(&time, &tm);
                char buf[64];
                strftime(buf, sizeof(buf), m_fmt.c_str(), &tm);
                str.append(buf);
                break;
            }
            case 't': // 线程id
            {
                str.append(std::to_string(event->getThreadId()));
                break;
            }
            case 'F': // 协程id
            {
                str.append(std::to_string(event->getFiberId()));
                break;
            }
            case 'p': // 日志级别
            {
                str.append(LogLevel::toString(level));
                break;
            }
            case 'm': // 日志内容
            {
                str.append(event->getContent());
                break;
            }
            case 'r': // 线程启动至今的秒数
            {
                str.append(bifang::time_difference(event->getElapse()));
                break;
            }
            case 'c': // 日志名称
            {
                str.append(event->getLogger()->getName());
                break;
            }
            case 'f': // 文件名
            {
                str.append(event->getFile());
                break;
            }
            case 'l': // 行号
            {
                str.append(std::to_string(event->getLine()));
                break;
            }
            case 'N': // 线程名称
            {
                str.append(event->getThreadName());
                break;
            }
            default:
            {
                str.append(i.second);
                break;
            }
        }
    }
}

// private
void LogFormatter::init()
{
    static std::unordered_set<std::string> match =
        {"n", "T", "d", "t", "F", "p", "m", "r", "c", "f", "l", "N", " "};

    std::vector<std::pair<std::string, std::string> > vec;
    std::string str;
    for (size_t i = 0; i < m_pattern.size(); i++)
    {
        if (m_pattern[i] != '%') // no format
        {
            str += m_pattern[i];
            continue;
        }
        i++; // skip % prefix
        if (i >= m_pattern.size()) // end
            break;
        if (m_pattern[i] == '%') // print %
        {
            str += '%';
            continue;
        }
        if (!str.empty()) // print normal string
        {
            vec.push_back(std::make_pair(" ", str));
            str.clear();
        }
        if (!isalpha(m_pattern[i])) // error, no match format
        {
            vec.push_back(std::make_pair("", std::string(&m_pattern[i - 1], &m_pattern[i + 1])));
            m_error = true;
            continue;
        }

        size_t n = i + 1;
        bool status = false;
        for (; n < m_pattern.size(); n++)
        {
            if (!status)
            {
                if (m_pattern[n] == '{')
                {
                    status = true;
                    continue;
                }
                else
                    break;
            }
            else
            {
                if (m_pattern[n] == '}')
                {
                    status = false;
                    n++;
                    break;
                }
            }
        }

        if (!status)
        {
            if (n == i + 1)
                vec.push_back(std::make_pair(std::string(&m_pattern[i], &m_pattern[i + 1]), ""));
            else
                vec.push_back(std::make_pair(std::string(&m_pattern[i], &m_pattern[i + 1]), std::string(&m_pattern[i + 2], &m_pattern[n - 1])));
        }
        else // format string is error
        {
            vec.push_back(std::make_pair("", std::string(&m_pattern[i - 1], &m_pattern[n])));
            m_error = true;
        }

        i = n - 1;
    }

    if (!str.empty()) // print normal string
        vec.push_back(std::make_pair(" ", str));

    for (auto& i : vec)
    {
        if (i.first.empty()) // error
        {
            std::cout << "parse format error: " << i.second <<  std::endl;
        }
        else
        {
            auto it = match.find(i.first);
            if (it != match.end())
                m_items.push_back(std::make_pair((i.first)[0], i.second));
            else
            {
                std::cout << "parse format error: " << "%" + i.first + i.second << std::endl;
                m_error = true;
            }
        }
    }
}


// 日志器类
Logger::Logger(const std::string& name)
    :m_name(name)
{
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        if (!m_appenders.empty())
        {
            for (auto& i : m_appenders)
                i->log(level, event);
        }
        else if (m_root)
            m_root->log(level, event);
    }
}

void Logger::add(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    m_appenders.push_back(appender);
}

bool Logger::del(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
    {
        if (*it == appender)
        {
            m_appenders.erase(it);
            return true;
        }
    }
    return false;
}

void Logger::clear()
{
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

std::string Logger::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    root["name"] = m_name;
    root["level"] = LogLevel::toString(m_level);

    for (auto& i : m_appenders)
        root["appenders"].append(Json::Value(i->toJsonString()));

    RESEQUENCE(root);
}

void Logger::reopen()
{
    MutexType::Lock lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
        (*it)->reopen();
}

// 控制台输出类
void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        m_formatter->format(std::cout, level, event);
    }
}

std::string StdoutLogAppender::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    root["type"] = "stdout";
    root["level"] = LogLevel::toString(m_level);
    root["formatter"] = m_formatter->getPattern();

    RESEQUENCE(root);
}


// 文件输出类
FileLogAppender::FileLogAppender(const std::string& filename,
                     uint64_t rolling_time, const std::string& rolling_dir)
    :m_filename(filename)
    ,m_rolling_time(rolling_time)
    ,m_rolling_dir(rolling_dir)
{
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

void FileLogAppender::rolling()
{
    MutexType::Lock lock(m_mutex);
    m_filestream.close();
    std::string new_name;
    std::string filename = m_filename;
    size_t pos = filename.rfind("/");
    if (pos != std::string::npos)
        filename = filename.substr(pos + 1, filename.size() - pos - 1);
    pos = filename.rfind(".");
    if (pos != std::string::npos)
    {
        std::string str1 = filename.substr(0, pos);
        std::string str2 = filename.substr(pos, filename.size() - pos);
        new_name = m_rolling_dir + str1 + "~" + time_to_string() + str2;
    }
    else
        new_name = m_rolling_dir + filename + "~" + time_to_string();
    //std::cout << m_filename << "->" << new_name << std::endl;
    FileUtil::Mv(m_filename, new_name);
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        m_formatter->format(m_filestream, level, event);
        m_filestream.flush();
    }
}

std::string FileLogAppender::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    root["type"] = "file";
    root["file"] = m_filename;
    root["level"] = LogLevel::toString(m_level);
    root["formatter"] = m_formatter->getPattern();

    RESEQUENCE(root);
}

void FileLogAppender::reopen()
{
    MutexType::Lock lock(m_mutex);
    if (UNLIKELY(m_flag))
    {
        IOManager* iom = IOManager::getThis();
        if (iom != nullptr)
        {
            m_flag = false;
            if (m_rolling_time != 0 && !m_rolling_dir.empty())
            {
                FileUtil::Mkdir(m_rolling_dir);
                iom->addTimer(m_rolling_time * 1000, std::bind(&FileLogAppender::rolling, this), true);
            }
        }
    }

    if (m_filestream)
        m_filestream.close();
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}


// 异步输出类
AsyncLogAppender::AsyncLogAppender(const std::string& filename, uint64_t interval,
                      uint64_t rolling_time, const std::string& rolling_dir)
    :m_filename(filename)
    ,m_interval(interval)
    ,m_rolling_time(rolling_time)
    ,m_rolling_dir(rolling_dir)
    ,m_thread(new Thread(std::bind(&AsyncLogAppender::run, this), "async_log"))
{
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

AsyncLogAppender::~AsyncLogAppender()
{
    m_running = false;
    m_thread->join();
    m_filestream.flush();
}

void AsyncLogAppender::rolling()
{
    MutexType::Lock lock(m_mutex);
    m_filestream.close();
    std::string new_name;
    std::string filename = m_filename;
    size_t pos = filename.rfind("/");
    if (pos != std::string::npos)
        filename = filename.substr(pos + 1, filename.size() - pos - 1);
    pos = filename.rfind(".");
    if (pos != std::string::npos)
    {
        std::string str1 = filename.substr(0, pos);
        std::string str2 = filename.substr(pos, filename.size() - pos);
        new_name = m_rolling_dir + str1 + "~" + time_to_string() + str2;
    }
    else
        new_name = m_rolling_dir + filename + "~" + time_to_string();
    //std::cout << m_filename << "->" << new_name << std::endl;
    FileUtil::Mv(m_filename, new_name);
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

void AsyncLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        std::string str;
        m_formatter->format(str, level, event);
        LogBuffer::ptr buffer;
        if (m_buffers.size() > 0)
        {
            buffer = m_buffers.back();
            if (buffer->avail() < str.size())
            {
                m_buffers.push_back(LogBuffer::ptr(new LogBuffer));
                buffer = m_buffers.back();
            }
        }
        else
        {
            m_buffers.push_back(LogBuffer::ptr(new LogBuffer));
            buffer = m_buffers.back();
        }
        buffer->write(str);
    }
}

std::string AsyncLogAppender::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    root["type"] = "async";
    root["file"] = m_filename;
    root["interval"] = m_interval;
    root["level"] = LogLevel::toString(m_level);
    root["formatter"] = m_formatter->getPattern();

    RESEQUENCE(root);
}

void AsyncLogAppender::reopen()
{
    MutexType::Lock lock(m_mutex);
    if (UNLIKELY(m_flag))
    {
        IOManager* iom = IOManager::getThis();
        if (iom != nullptr)
        {
            m_flag = false;
            if (m_rolling_time != 0 && !m_rolling_dir.empty())
            {
                FileUtil::Mkdir(m_rolling_dir);
                iom->addTimer(m_rolling_time * 1000, std::bind(&AsyncLogAppender::rolling, this), true);
            }
        }
    }

    if (m_filestream)
        m_filestream.close();
    FileUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

// private
void AsyncLogAppender::run()
{
    std::vector<LogBuffer::ptr> buffers;
    while (m_running || m_buffers.size() > 0)
    {
        if (m_buffers.size() > 0)
        {
            MutexType::Lock lock(m_mutex);
            buffers.swap(m_buffers);
            lock.unlock();

            for (auto buf : buffers)
                m_filestream.write(buf->data(), buf->length());
            m_filestream.flush();
            buffers.clear();
        }

        usleep(m_interval * 1000);
    }
}


// UDP输出类
UDPLogAppender::UDPLogAppender(const std::string& addrstr, uint64_t interval)
    :m_addrstr(addrstr)
    ,m_interval(interval)
    ,m_thread(new Thread(std::bind(&UDPLogAppender::run, this), "udp_log"))
{
    m_addr = Address::getAddrInfo(m_addrstr);
    if (m_addr == nullptr)
        throw std::logic_error("log udp output address is invaild!");
    m_socket = Socket::createUDP(m_addr);
}

UDPLogAppender::~UDPLogAppender()
{
    m_running = false;
    m_thread->join();
}

bool UDPLogAppender::redirect(const std::string& addrstr)
{
    MutexType::Lock lock(m_mutex);
    auto addr = Address::getAddrInfo(addrstr);
    if (addr == nullptr)
        return false;
    m_addrstr = addrstr;
    m_addr = addr;
    m_socket = Socket::createUDP(m_addr);
}

void UDPLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        std::string str;
        m_formatter->format(str, level, event);
        LogBuffer::ptr buffer;
        if (m_buffers.size() > 0)
        {
            buffer = m_buffers.back();
            if (buffer->avail() < str.size())
            {
                m_buffers.push_back(LogBuffer::ptr(new LogBuffer));
                buffer = m_buffers.back();
            }
        }
        else
        {
            m_buffers.push_back(LogBuffer::ptr(new LogBuffer));
            buffer = m_buffers.back();
        }
        buffer->write(str);
    }
}

std::string UDPLogAppender::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    root["type"] = "udp";
    root["address"] = m_addrstr;
    root["interval"] = m_interval;
    root["level"] = LogLevel::toString(m_level);
    root["formatter"] = m_formatter->getPattern();

    RESEQUENCE(root);
}

// private
void UDPLogAppender::run()
{
    std::vector<LogBuffer::ptr> buffers;
    while (m_running || m_buffers.size() > 0)
    {
        if (m_buffers.size() > 0)
        {
            MutexType::Lock lock(m_mutex);
            buffers.swap(m_buffers);
            lock.unlock();

            for (auto buf : buffers)
                m_socket->sendto(buf->data(), buf->length(), m_addr);

            buffers.clear();
        }

        usleep(m_interval * 1000);
    }
}


// 日志器管理类
LoggerManager::LoggerManager()
{
    m_root.reset(new Logger("root"));
    m_root->add(LogAppender::ptr(new StdoutLogAppender));

    static bifang::Config<std::set<LogDefine> >::ptr g_log_config =
        bifang::ConfigMgr::GetInstance()->get("logs", std::set<LogDefine>(), "logs config");

    // 添加日志配置回调函数
    g_log_config->addCallback(
            [](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value)
            {
                //std::cout << "logger conf changed" << std::endl;
                for (auto& i : new_value)
                {
                    auto it = old_value.find(i);
                    bifang::Logger::ptr logger;
                    if (it != old_value.end() && i == *it) // 无需修改
                        continue;

                    logger = LoggerMgr::GetInstance()->get(i.name);

                    logger->setLevel(i.level);

                    logger->clear();
                    for (auto& appender : i.appenders)
                    {
                        bifang::LogAppender::ptr ap;
                        if (appender.type == 1)
                            ap.reset(new StdoutLogAppender);
                        else if (appender.type == 2)
                            ap.reset(new FileLogAppender(appender.file, appender.rolling_time, appender.rolling_dir));
                        else if (appender.type == 3)
                            ap.reset(new AsyncLogAppender(appender.file, appender.interval, appender.rolling_time, appender.rolling_dir));
                        else if (appender.type == 4)
                            ap.reset(new UDPLogAppender(appender.address, appender.interval));

                        ap->setLevel(appender.level);
                        if (appender.formatter.empty())
                        {
                            std::cout << "log.name=" << i.name
                                << ", appender type=" << appender.type
                                << " is no formatter, will be using default formatter"
                                << std::endl;
                        }
                        else
                        {
                            LogFormatter::ptr fmt(new LogFormatter(appender.formatter));
                            if (!fmt->isError())
                                ap->setFormatter(fmt);
                            else
                            {
                                std::cout << "log.name=" << i.name
                                    << ", appender type=" << appender.type
                                    << ", formatter=" << appender.formatter
                                    << " is invalid, will be using default formatter"
                                    << std::endl;
                            }
                        }

                        logger->add(ap);
                    }
                }

                for (auto& i : old_value)
                {
                    auto it = new_value.find(i);
                    if (it == new_value.end()) // 删除无效的logger
                    {
                        auto logger = LoggerMgr::GetInstance()->get(i.name);
                        logger->setLevel(LogLevel::UNKNOW);
                        logger->clear();
                    }
                }
            });
}

Logger::ptr LoggerManager::get(const std::string& name)
{
    MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name);

    if (it != m_loggers.end())
        return it->second;

    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root; 
    m_loggers[name] = logger;

    return logger;
}

std::string LoggerManager::toJsonString()
{
    MutexType::Lock lock(m_mutex);
    Json::Value root;
    for (auto& i : m_loggers)
        root.append(Json::Value(i.second->toJsonString()));

    RESEQUENCE(root);
}

void LoggerManager::reopen()
{
    MutexType::Lock lock(m_mutex);
    for (auto it = m_loggers.begin(); it != m_loggers.end(); it++)
    {
        Logger::ptr logger = it->second;
        logger->reopen();
    }
}


// 强制使单例生效
struct LoggerInit
{
    LoggerInit()
    {
        LoggerMgr::GetInstance();
    }
};

static LoggerInit g_logger_init;

}
