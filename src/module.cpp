#include "module.h"
#include "log.h"
#include "config.h"
#include "environment.h"
#include "util.h"

namespace bifang
{

SystemLogger();

static Config<std::string>::ptr g_module_path =
           ConfigMgr::GetInstance()->get("module.path", std::string("module"), "module path");

Module::Module(const std::string& name, const std::string& version)
    :m_name(name)
    ,m_version(version)
    ,m_id(name + "/" + version)
{
}

// virtual
bool Module::onLoad()
{
    log_debug << "Module OnLoad";
    return true;
}

// virtual
bool Module::onUnload()
{
    log_debug << "Module OnUnload";
    return true;
}

// virtual
bool Module::onConnect(Stream::ptr stream)
{
    log_debug << "Module OnConnect";
    return true;
}

// virtual
bool Module::onDisconnect(Stream::ptr stream)
{
    log_debug << "Module OnDisconnect";
    return true;
}

// virtual
bool Module::onServerReady()
{
    log_debug << "Module OnServerReady";
    return true;
}

// virtual
bool Module::onServerUp()
{
    log_debug << "Module OnServerUp";
    return true;
}

// virtual
std::string Module::toString()
{
    std::stringstream ss;
    ss << "[Module name=" << getName()
       << ", version=" << getVersion()
       << ", filename=" << getFilename()
       << "]";
    return ss.str();
}


typedef Module* (*create_module)();
typedef void (*destory_module)(Module*);

class ModuleCloser
{
public:
    ModuleCloser(void* handle, destory_module d)
        :m_handle(handle)
        ,m_destory(d)
    {
    }

    void operator()(Module* module)
    {
        std::string name = module->getName();
        std::string version = module->getVersion();
        std::string path = module->getFilename();
        m_destory(module);
        int ret = dlclose(m_handle);
        if (ret)
        {
            log_error << "dlclose handle fail name=" << name
                << ", version=" << version
                << ", path=" << path
                << ", handle=" << m_handle
                << ", error=" << dlerror();
        }
        else
        {
            log_info << "destory module=" << name
                << ", version=" << version
                << ", path=" << path
                << ", handle=" << m_handle
                << " success";
        }
    }

private:
    void* m_handle;
    destory_module m_destory;
};

static Module::ptr GetModule(const std::string& path)
{
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle)
    {
        log_error << "cannot load library("<< path << "), error=" << dlerror();
        return nullptr;
    }

    create_module create = (create_module)dlsym(handle, "CreateModule");
    if (!create)
    {
        log_error << "cannot load symbol CreateModule in library(" << path
            << "), error=" << dlerror();
        dlclose(handle);
        return nullptr;
    }

    destory_module destory = (destory_module)dlsym(handle, "DestoryModule");
    if (!destory)
    {
        log_error << "cannot load symbol DestoryModule in library(" << path
            << "), error=" << dlerror();
        dlclose(handle);
        return nullptr;
    }

    Module::ptr module(create(), ModuleCloser(handle, destory));
    module->setFilename(path);
    log_info << "load module name=" << module->getName()
        << ", version=" << module->getVersion()
        << ", filename=" << module->getFilename()
        << " success";
    return module;
}


void ModuleManager::init()
{
    auto path = EnvMgr::GetInstance()->getAbsolutePath(g_module_path->getValue());

    std::vector<std::string> files;
    FileUtil::ListAllFile(files, path, ".so");

    log_debug << "module path: " << path << ", module number: " << files.size();
    for (auto& i : files)
        initModule(i);
}

void ModuleManager::add(Module::ptr m)
{
    del(m->getId());
    RWMutexType::WriteLock lock(m_mutex);
    m_modules[m->getId()] = m;
}

void ModuleManager::del(const std::string& name)
{
    Module::ptr module;
    RWMutexType::WriteLock lock(m_mutex);
    auto it = m_modules.find(name);
    if (it == m_modules.end())
        return;

    module = it->second;
    m_modules.erase(it);
    lock.unlock();
    module->onUnload();
}

void ModuleManager::del()
{
    RWMutexType::ReadLock lock(m_mutex);
    auto tmp = m_modules;
    lock.unlock();

    for (auto& i : tmp)
        del(i.first);
}

Module::ptr ModuleManager::get(const std::string& name)
{
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_modules.find(name);
    return it == m_modules.end() ? nullptr : it->second;
}

void ModuleManager::getList(std::vector<Module::ptr>& ms)
{
    RWMutexType::ReadLock lock(m_mutex);
    for (auto& i : m_modules)
        ms.push_back(i.second);
}

void ModuleManager::initModule(const std::string& path)
{
    Module::ptr m = GetModule(path);
    if (m)
        add(m);
}


}
