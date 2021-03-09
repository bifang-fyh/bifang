/**
 * brief: 协程切换效率简单测试
 */
#include "bifang.h"

RootLogger();

static uint64_t count = 0;

void run()
{
    while (1)
    {
        count++;
        bifang::Fiber::getThis()->back();
    }
}

int main(int argc, char* argv[])
{
    uint64_t begin = bifang::getCurrentMS();
    bifang::Fiber::getThis();
    bifang::Fiber::ptr fiber(new bifang::Fiber(run, 0, true));

    while (count < 10000000)
    {
        count++;
        fiber->call();
    }

    uint64_t end = bifang::getCurrentMS();
    
    log_info << "execute " << count << " switch need " << end - begin << "ms";

    return 0;
}
