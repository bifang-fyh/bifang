#include "environment.h"
#include "log.h"
#include "config.h"

namespace bifang
{

SystemLogger();


bool Enviroment::init(int argc, char** argv)
{
    char link[512] = {0};
    char path[512] = {0};
    sprintf(link, "/proc/%d/exe", getpid());
    readlink(link, path, sizeof(path));
    // /path/可执行文件
    m_exe = path;

    // /path/
    auto pos = m_exe.find_last_of("/");
    m_cwd = m_exe.substr(0, pos) + "/";

    m_program = argv[0];
    // -config /path/to/config -file xxxx -d
    const char* now_key = nullptr;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-') // -后面连着的字符串为参数
        {
            if (strlen(argv[i]) > 1)
            {
                if (now_key)
                    add(now_key, "");

                now_key = argv[i] + 1;
            }
            else
            {
                log_error << "invalid arg idx=" << i
                    << ", val=" << argv[i];
                return false;
            }
        }
        else // 参数值
        {
            if (now_key)
            {
                add(now_key, argv[i]);
                now_key = nullptr;
            }
            else
            {
                log_error << "invalid arg idx=" << i
                    << ", val=" << argv[i];
                return false;
            }
        }
    }
    if (now_key)
        add(now_key, "");

    return true;
}

void Enviroment::add(const std::string& name, const std::string& val)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_args[name] = val;
}

bool Enviroment::has(const std::string& name)
{
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_args.find(name);
    return it != m_args.end();
}

void Enviroment::del(const std::string& name)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_args.erase(name);
}

std::string Enviroment::get(const std::string& name,
                const std::string& default_value)
{
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_args.find(name);
    return it != m_args.end() ? it->second : default_value;
}

void Enviroment::printArgs()
{
    RWMutexType::ReadLock lock(m_mutex);
    std::cout << "executable file: " << m_program << std::endl
        << "param:" << std::endl;
    for (auto& i : m_args)
        std::cout << "      " << "-" << i.first << " : " << i.second << std::endl;
}

void Enviroment::addHelp(const std::string& name, const std::string& desc)
{
    removeHelp(name);
    RWMutexType::WriteLock lock(m_mutex);
    m_helps.push_back(std::make_pair(name, desc));
}

void Enviroment::removeHelp(const std::string& name)
{
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_helps.begin(); it != m_helps.end();)
    {
        if (it->first == name)
            it = m_helps.erase(it);
        else
            it++;
    }
}

void Enviroment::printHelp()
{
    RWMutexType::ReadLock lock(m_mutex);
    std::cout << "Usage: " << m_program << " [options]" << std::endl;
    for (auto& i : m_helps)
    {
        std::cout << std::setw(5) << "-" << i.first << " : "
            << i.second << std::endl;
    }
}

bool Enviroment::setEnv(const std::string& name, const std::string& value)
{
    return !setenv(name.c_str(), value.c_str(), 1);
}

std::string Enviroment::getEnv(const std::string& name,
    const std::string& default_value)
{
    char* env = getenv(name.c_str());
    if (env == nullptr)
        return default_value;
    return env;
}

std::string Enviroment::getAbsolutePath(const std::string& path) const
{
    if (path.empty())
        return "";
    if (path[0] == '/')
        return path;
    return m_cwd + path;
}

std::string Enviroment::getConfigPath()
{
    return getAbsolutePath(get("c", ""));
}

}
