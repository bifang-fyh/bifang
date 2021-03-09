/**
 * brief: 多线程简单测试
 */
#include "bifang.h"

RootLogger();
//uint64_t g_count;
bifang::Mutex g_mutex;
static std::atomic<uint64_t> g_count = {0};

void run()
{
    log_info << "thread name=" << bifang::Thread::getName()
        << ", id=" << bifang::Thread::getId();

    for (size_t i = 0; i < 1000000; i++)
    {
        //bifang::Mutex::Lock lock(g_mutex);
        g_count++;
    }
}

int main(int argc, char *argv[])
{
    std::vector<bifang::Thread::ptr> thrs;
    for (size_t i = 0; i < 5; i++)
    {
        bifang::Thread::ptr thr(new bifang::Thread(&run, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    
    for (size_t i = 0; i < thrs.size(); i++)
        thrs[i]->join();

    log_info << "g_count=" << g_count;

    return 0;
}
