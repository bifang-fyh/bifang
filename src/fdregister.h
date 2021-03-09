/**
 * brief: 文件句柄注册管理(可对所有的fd进行统一管理, 在hook中实现协程方式的非阻塞)
 */
#ifndef __BIFANG_FDREGISTER_H
#define __BIFANG_FDREGISTER_H

#include <memory>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "singleton.h"
#include "lock.h"

namespace bifang
{

/**
 * brief: 文件句柄类
 * details: 管理文件句柄类型(读/写超时时间)
 */
class FdData
{
public:
    typedef std::shared_ptr<FdData> ptr;

    /**
     * brief: 通过文件句柄构造FdCtx
     */
    FdData(int fd);

    /**
     * brief: 获取超时时间
     * param: type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
     * return: 超时时间毫秒
     */
    int64_t getTimeout(int type);

    /**
     * brief: 设置超时时间
     * param: type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
     *        v 时间毫秒
     */
    void setTimeout(int type, int64_t v);

private:
    // 文件句柄
    int m_fd;
    // 读超时时间毫秒
    int64_t m_recvTimeout;
    // 写超时时间毫秒
    int64_t m_sendTimeout;
};

/**
 * brief: 文件句柄管理类
 */
class FdManager
{
public:
    typedef RWMutex RWMutexType;

    FdManager();

    /**
     * brief: 获取文件句柄类
     * param: fd 文件句柄
     * return: 返回对应文件句柄类的智能指针
     */
    FdData::ptr get(int fd);

    /**
     * brief: 注册文件句柄类
     * param: fd 文件句柄
     * return: true  - 成功
     *         false - 失败
     */
    bool add(int fd);

    /**
     * brief: 删除文件句柄类
     * param: fd 文件句柄
     * return: true  - 成功
     *         false - 失败
     */
    bool del(int fd);

private:
    // 读写锁
    RWMutexType m_mutex;
    // 文件句柄集合
    std::vector<FdData::ptr> m_datas;
};

// 文件句柄单例
typedef Singleton<FdManager> FdMgr;

}

#endif /*__BIFANG_FDREGISTER_H*/
