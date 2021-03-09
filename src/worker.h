/**
 * brief: iomanager生成器
 */
#ifndef __BIFANG_WORKER_H
#define __BIFANG_WORKER_H

#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "iomanager.h"
#include "lock.h"
#include "singleton.h"
#include "log.h"

namespace bifang
{

class WorkerManager
{
public:
    /**
     * brief: 将调度器s加入工作列表中
     */
    void add(Scheduler::ptr s);
    /**
     * brief: 获取名字为name的调度器
     * return: 返回对应名字的调度器, 若不存在则返回nullptr
     */
    Scheduler::ptr get(const std::string& name);
    /**
     * brief: 删除名字为name的调度器
     */
    void del(const std::string& name);
    /**
     * brief: 清空调度器列表
     */
    void clear();
    /**
     * brief: 获取名字为name的调度器, 并将其转换为IOManager::ptr
     */
    IOManager::ptr getAsIOManager(const std::string& name);

    bool init(const std::unordered_map<std::string, uint32_t>& v);

    uint32_t total() const { return m_datas.size(); };

    std::string toString();

private:
    std::unordered_map<std::string, Scheduler::ptr> m_datas;
};

typedef Singleton<WorkerManager> WorkerMgr;

}

#endif /*__BIFANG_WORKER_H*/
