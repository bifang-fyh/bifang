/**
 * brief: 协程简单测试(call 和 back 可以不涉及调度器, SwapIn 和 SwapOut 需借助调度器来执行)
 */
#include "bifang.h"

RootLogger();


void run_in_fiber()
{
    log_info << "run_in_fiber 1";
    bifang::Fiber::getThis()->back();
    log_info << "run_in_fiber 2";
}

/**
 * details: test_fiber(call) -> run_in_fiber(back) -> test_fiber(call) -> run_in_fiber(on Filber::main_run to back) -> test_fiber(end)
 */
void test_fiber()
{
    log_info << "test_fiber begin";
    {
        bifang::Fiber::getThis();
        bifang::Fiber::ptr fiber(new bifang::Fiber(run_in_fiber, true));
        fiber->call();
        log_info << "test_fiber after call 1";
        fiber->call();
        log_info << "test_fiber after call 2";
    }
    log_info << "test_fiber end";
}

int main(int argc, char* argv[])
{
    std::vector<bifang::Thread::ptr> thrs;
    for (int i = 0; i < 5; i++)
        thrs.push_back(bifang::Thread::ptr(new bifang::Thread(&test_fiber, "name_" + std::to_string(i))));
    for (auto i : thrs)
        i->join();

    return 0;
}
