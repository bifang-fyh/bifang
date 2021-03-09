#include "worker.h"
#include "config.h"


namespace bifang
{

void WorkerManager::add(Scheduler::ptr s)
{
    m_datas[s->getName()] = s;
}

Scheduler::ptr WorkerManager::get(const std::string& name)
{
    auto it = m_datas.find(name);
    if (it == m_datas.end())
        return nullptr;
    return it->second;
}

void WorkerManager::del(const std::string& name)
{
    auto it = m_datas.find(name);
    if (it == m_datas.end())
        return;
    m_datas.erase(it);
}

void WorkerManager::clear()
{
    for (auto& i : m_datas)
    {
        i.second->schedule([](){});
        i.second->stop();
    }
    m_datas.clear();
}

IOManager::ptr WorkerManager::getAsIOManager(const std::string& name)
{
    return std::dynamic_pointer_cast<IOManager>(get(name));
}

bool WorkerManager::init(const std::unordered_map<std::string, uint32_t>& v)
{
    for (auto& i : v)
    {
        Scheduler::ptr s = std::make_shared<IOManager>(i.second, false, i.first);
        add(s);
    }
    return true;
}

std::string WorkerManager::toString()
{
    std::stringstream ss;
    ss << "workers numbers: " << total();
    for (auto& i : m_datas)
    {
        ss << std::endl << "    " << i.first << ": " << i.second->toString();
    }
    return ss.str();
}

}
