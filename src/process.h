/**
 * brief: 进程管理文件
 */
#ifndef __BIFANG_PROCESS_H
#define __BIFANG_PROCESS_H

#include <functional>
#include <string>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#include "singleton.h"


namespace bifang
{

class ProcessManager
{
public:
    std::string toString() const;

    /**
     * brief: 启动程序可以选择用守护进程的方式
     * param: argc 参数个数
     *        argv 参数值数组
     *        main_cb 启动函数
     *        is_daemon 是否守护进程的方式
     * return: 返回程序的执行结果
     */
    int start(int argc, char** argv, 
            std::function<int(int argc, char** argv)> main_cb, bool is_daemon);

public:
    pid_t getParentId()         const { return m_parentId;        }
    pid_t getMainId()           const { return m_mainId;          }
    time_t getParentStartTime() const { return m_parentStartTime; }
    time_t getMainStartTime()   const { return m_mainStartTime;   }
    uint32_t getRestartCount()  const { return m_restartCount;    }

private:
    int normalStart(int argc, char** argv,
            std::function<int(int argc, char** argv)> main_cb);

    int daemonStart(int argc, char** argv,
            std::function<int(int argc, char** argv)> main_cb);

private:
    // 父进程id
    pid_t m_parentId = 0;
    // 主进程id
    pid_t m_mainId = 0;
    // 父进程启动时间
    time_t m_parentStartTime = 0;
    // 主进程启动时间
    time_t m_mainStartTime = 0;
    // 主进程重启的次数
    uint32_t m_restartCount = 0;
};

typedef Singleton<ProcessManager> ProcessMgr;

}

#endif /*__BIFANG_PROCESS_H*/
