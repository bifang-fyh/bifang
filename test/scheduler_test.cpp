/**
 * brief: 调度器简单测试
 */
#include "bifang.h"

RootLogger();

void fun()
{
}

void test()
{
    static int s_count = 5;
    bifang::hook_disable();
    log_info << "test in fiber s_count=" << s_count;

    // 测试fiber id是否会回收
    bifang::Fiber::ptr f1(new bifang::Fiber(&fun));
    bifang::Fiber::ptr f2(new bifang::Fiber(&fun));
    bifang::Fiber::ptr f3(new bifang::Fiber(&fun));

    usleep(500000);
    if (--s_count >= 0)
        bifang::Scheduler::getThis()->schedule(&test);
}

int main(int argc, char* argv[])
{
    log_info << "main begin";
    bifang::Scheduler sc(5, true, "test");
    // 没有iomanager, 必须关闭hook, 不然usleep会跑飞
    bifang::hook_disable();
    sc.start();
    usleep(500000);
    sc.schedule(&test);
    sc.stop();
    log_info << "main end";

    return 0;
}
