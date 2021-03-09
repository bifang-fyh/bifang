/**
 * brief: daemon简单测试
 */
#include "bifang.h"

RootLogger();

bifang::Timer::ptr timer;

int server_main(int argc, char* argv[])
{
    log_info << bifang::ProcessMgr::GetInstance()->toString();
    bifang::IOManager iom(1);
    timer = iom.addTimer(1000,
        [&](){
                log_info << "OnTimer";
                static int count = 0;
                if (++count > 7)
                    timer->cancel(); // 取消定时器, 使子进程正常退出, 如果是设置当前程序剥离控制台的话, 简单exit掉子进程会导致无限重启, 具体查看process.cpp文件
            }, true);

    return 0;
}

int main(int argc, char* argv[])
{
    return bifang::ProcessMgr::GetInstance()->start(argc, argv, server_main, true);
}
