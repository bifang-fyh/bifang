/**
 * brief: 日志器简单测试
 */
#include "bifang.h"
 
NameLogger("test");

void StdoutLog()
{
    uint64_t count = 0;
    uint64_t before = bifang::getCurrentMS();

    while (count < 5000)
    {
        log_debug << "test " << ++count;
        log_info << "test " << ++count;
        log_warn << "test " << ++count;
        log_error << "test " << ++count;
        log_fatal << "test " << ++count;
    }

    uint64_t after = bifang::getCurrentMS();
    std::cout << "output " << count << " message need " << after - before << "ms" << std::endl;
}

void log_test()
{
    uint64_t count = 0;
    uint64_t before = bifang::getCurrentMS();

    while (count < 500000)
        log_info << "test " << ++count;

    uint64_t after = bifang::getCurrentMS();
    std::cout << "output " << count << " message need " << after - before << "ms" << std::endl;
}

void FileLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::FileLogAppender::ptr appender(new bifang::FileLogAppender("logs/file_log.txt", 0, ""));
    log->add(appender);
    log_test();
}

void AsyncLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::AsyncLogAppender::ptr appender(new bifang::AsyncLogAppender("logs/async_log.txt", 4000, 0, ""));
    log->add(appender);
    log_test();
}

void UDPLog()
{
    bifang::Logger::ptr log = bifang::LoggerMgr::GetInstance()->get("test");
    bifang::UDPLogAppender::ptr appender(new bifang::UDPLogAppender("127.0.0.1:7777", 4000));
    log->add(appender);
    log_test();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Please input log mode!" << std::endl;
        return -1;
    }
    
    if (!strcmp(argv[1], "0"))
        StdoutLog();
    else if (!strcmp(argv[1], "1"))
        FileLog();
    else if (!strcmp(argv[1], "2"))
        AsyncLog();
    else if (!strcmp(argv[1], "3"))
        UDPLog();
    else
    {
        std::cout << "log mode input fail(vaild: 0, 1, 2, 3)" << std::endl;
        return -1;
    }

    return 0;
}

