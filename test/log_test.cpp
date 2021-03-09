/**
 * brief: 日志简单
 */
#include "bifang.h"
 
NameLogger("test");

void log_test()
{
    uint64_t count = 0;
    uint64_t before = bifang::getCurrentMS();

    while (count < 500000)
        log_info << "test " << ++count;

    uint64_t after = bifang::getCurrentMS();
    std::cout << "output " << count << " message need " << after - before << "ms" << std::endl;
}

void SetFileLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::FileLogAppender::ptr appender(new bifang::FileLogAppender("logs/file_log.txt"));
    log->add(appender);
}

void SetAsyncLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::AsyncLogAppender::ptr appender(new bifang::AsyncLogAppender("logs/async_log.txt", 4000));
    log->add(appender);
}

void SetUDPLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::UDPLogAppender::ptr appender(new bifang::UDPLogAppender("127.0.0.1:7777", 4000));
    log->add(appender);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Please input log mode!" << std::endl;
        return -1;
    }
    
    if (!strcmp(argv[1], "0"))
        SetFileLog();
    else if (!strcmp(argv[1], "1"))
        SetAsyncLog();
    else if (!strcmp(argv[1], "2"))
        SetUDPLog();
    else
    {
        std::cout << "log mode input fail(vaild: 0, 1, 2)" << std::endl;
        return -1;
    }

    log_test();

    return 0;
}
