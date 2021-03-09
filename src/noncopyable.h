#ifndef __BIFANG_NONCOPYABLE_H
#define __BIFANG_NONCOPYABLE_H

namespace bifang
{

struct Noncopyable
{
    Noncopyable() = default;

    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;

    Noncopyable& operator=(const Noncopyable&) = delete;
};

}

#endif /*__NONCOPYABLE_H*/

