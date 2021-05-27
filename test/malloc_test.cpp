/**
 * brief: tcmalloc性能简单测试
 */
#include "bifang.h"
 
int main(int argc, char* argv[])
{
#define MALLOC_COUNT 50000000
    size_t start = bifang::getCurrentMS();
    std::string str = bifang::StringUtil::randomString(1000);

    for (size_t i = 0; i < MALLOC_COUNT; i++)
    {
        char* p = new char[1000];
        memcpy(p, &str[0], 1000);
        delete p;
    }

    std::cout << "new " << MALLOC_COUNT << " times need " << bifang::getCurrentMS() - start << "ms" << std::endl;

    return 0;
}

