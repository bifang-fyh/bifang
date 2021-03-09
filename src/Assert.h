#ifndef __BIFANG_ASSERT_H
#define __BIFANG_ASSERT_H

#include <assert.h>

#include "log.h"

namespace bifang
{

/**
 * brief: 获取当前栈信息的字符串
 * param: size 栈的最大层数
 *        skip 跳过栈顶的层数
 *        prefix 栈信息前输出的内容
 */
std::string backtrace(int size = 64, int skip = 2, const std::string& prefix = "");

}

#if defined __GUNC__

// 告诉编译器条件大概率成立
#define LIKELY(x)    __builtin_expect(!!(x), 1)
// 告诉编译器条件大概率不成立
#define UNLIKELY(x)  __builtin_expect(!!(x), 0)

#else

#define LIKELY(x)    (!!(x))
#define UNLIKELY(x)  (!!(x))

#endif


// 断言宏封装
#define ASSERT(x) \
    do \
    { \
        if (UNLIKELY(!(x))) \
        { \
            log_fatal << "ASSERT: " #x << std::endl << "backtrace:" \
                << std::endl << bifang::backtrace(50, 2, "    "); \
            assert(x); \
        } \
    } while (0)

// 断言宏封装(带用户自定义打印信息)
#define ASSERT_MSG(x, m) \
    do \
    { \
        if (UNLIKELY(!(x))) \
        { \
            log_fatal << "ASSERT: " #x << std::endl << m << std::endl \
                << "backtrace:" << std::endl << bifang::backtrace(50, 2, "    "); \
            assert(x); \
        } \
    } while (0)


#endif /*__BIFANG_ASSERT_H*/
