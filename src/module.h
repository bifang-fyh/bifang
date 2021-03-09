#ifndef __BIFANG_MODULE_H
#define __BIFANG_MODULE_H

#include <unordered_map>
#include <dlfcn.h>

#include "singleton.h"
#include "lock.h"
#include "stream.h"


namespace bifang
{

/**
 * brief: 模块基类
 */
class Module
{
public:
    typedef std::shared_ptr<Module> ptr;

    Module(const std::string& name, const std::string& version);

    virtual ~Module() {}

    virtual bool onLoad();
    virtual bool onUnload();

    virtual bool onConnect(Stream::ptr stream);
    virtual bool onDisconnect(Stream::ptr stream);

    virtual bool onServerReady();
    virtual bool onServerUp();

    virtual std::string toString();

public:
    const std::string& getName()     const { return m_name;     }
    const std::string& getVersion()  const { return m_version;  }
    const std::string& getFilename() const { return m_filename; }
    const std::string& getId()       const { return m_id;       }

    void setFilename(const std::string& v) { m_filename = v;    }

protected:
    // 模块名字
    std::string m_name;
    // 版本号
    std::string m_version;
    // 文件名(完整路径)
    std::string m_filename;
    // 模块id(名称/版本号)
    std::string m_id;
};

/**
 * brief: 模块管理类
 */
class ModuleManager
{
public:
    typedef RWMutex RWMutexType;

    void init();

    void add(Module::ptr m);
    void del(const std::string& name);
    void del();

    Module::ptr get(const std::string& name);

    void getList(std::vector<Module::ptr>& ms);

private:
    void initModule(const std::string& path);

private:
    RWMutexType m_mutex;
    std::unordered_map<std::string, Module::ptr> m_modules;
};

typedef Singleton<ModuleManager> ModuleMgr;

}

#endif /*__BIFANG_MODULE_H*/
