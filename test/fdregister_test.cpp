/**
 * brief: fdregister简单测试
 */
#include "bifang.h"

RootLogger();

bool g_running = true;

void print()
{
    while (g_running)
    {
        sleep(3);
        log_info << "How old are you? I fine thank you";
    }
}

void fd_test()
{
    // 可以在iomanager下实现非阻塞
    bifang::FdMgr::GetInstance()->add(STDOUT_FILENO);
    bifang::FdMgr::GetInstance()->add(STDIN_FILENO);
    
    // 无法在iomanager下实现非阻塞
    //int flags = g_sys_fcntl(STDOUT_FILENO, F_GETFL, 0);
    //if (flags == -1)
    //    ASSERT_MSG(false, "fcntl error, errno=" + std::to_string(errno)
    //        + ", errstr=" + strerror(errno));
    //if (!(flags & O_NONBLOCK))
    //    g_sys_fcntl(STDOUT_FILENO, F_SETFL, flags | O_NONBLOCK);
    //
    //flags = g_sys_fcntl(STDIN_FILENO, F_GETFL, 0);
    //if (flags == -1)
    //    ASSERT_MSG(false, "fcntl error, errno=" + std::to_string(errno)
    //        + ", errstr=" + strerror(errno));
    //if (!(flags & O_NONBLOCK))
    //    g_sys_fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    std::string buf;
    buf.resize(1024);
    int n;
    while (1)
    {
        n = read(STDIN_FILENO, &buf[0], buf.size());
        if (n > 0)
            write(STDOUT_FILENO, &buf[0], n);
        if (buf.find("quit") == 0 || buf.find("exit") == 0)
            break;
    }

    g_running = false;
}

int main(int argc, char* argv[])
{
    // 线程数量必须设置为1, 如果两个函数被不同的线程给调度走了, 那本来两个函数就互不冲突了也就无法看到测试效果了
    bifang::IOManager iom(1, true, "test");
    iom.schedule(&fd_test);
    iom.schedule(&print);

    return 0;
}
