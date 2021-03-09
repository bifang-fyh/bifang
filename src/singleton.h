/**
 * brief: 单例类封装
 */
#ifndef __BIFANG_SINGLETON_H
#define __BIFANG_SINGLETON_H

namespace bifang
{

template<typename T>
struct Singleton
{
    static T* GetInstance()
    {
        static T m;
        return &m;
    }
};


}


#endif
