/**
 * brief: 环境变量
 */
#ifndef __BIFANG_ENVIROMENT_H
#define __BIFANG_ENVIROMENT_H

#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>

#include "singleton.h"
#include "lock.h"

namespace bifang
{

class Enviroment
{
public:
    typedef RWMutex RWMutexType;

    /**
     * brief: 设置m_args, 并获取m_program, m_cwd, m_exe
     * param: argc 参数个数
     *        argv 参数列表
     * return: true  - 成功
     *         false - 失败
     */
    bool init(int argc, char** argv);

    /**
     * brief: 将name|val添加进去m_args
     * param: name 参数名
     *        val 参数值
     */
    void add(const std::string& name, const std::string& val);
    /**
     * brief: 判断name是否存在于m_args中
     * param: name 参数名
     * return: true  - name存在于m_args
     *         false - name不存在于m_args
     */
    bool has(const std::string& name);
    /**
     * brief: 删除m_args中的name
     * param: name 需要删除的参数名
     */
    void del(const std::string& name);
    /**
     * brief: 从m_args中取出name的数据
     * param: name 需要取出的参数名
     *        default_value 默认参数, 取不出数据则返回这个值
     * return: m_args对应的参数值
     */
    std::string get(const std::string& name,
                    const std::string& default_value = "");
    /**
     * brief: 打印m_args列表
     */
    void printArgs();

    /**
     * brief: 添加帮助信息
     * param: name 帮助信息名字
     *        desc 描述信息
     */
    void addHelp(const std::string& name, const std::string& desc);
    /**
     * brief: 删除帮助信息
     * param: name 帮助信息名字
     */
    void removeHelp(const std::string& name);
    /**
     * brief: 打印帮助信息
     */
    void printHelp();

    /**
     * brief: 获取可执行文件的完整路径
     */
    const std::string& getExe() const { return m_exe; }
    /**
     * brief: 获取可执行文件的运行目录
     */
    const std::string& getCwd() const { return m_cwd; }

    /**
     * brief: 设置环境变量
     * param: name 环境变量名
     *        value 环境变量值
     * return: true  - 成功
     *         false - 失败
     */
    bool setEnv(const std::string& name, const std::string& value);
    /**
     * brief: 获取环境变量
     * param: name 环境变量名
     *        default_value 默认值(找不到name的环境变量则返回这个值)
     */
    std::string getEnv(const std::string& name,
                    const std::string& default_value = "");

    /**
     * brief: 获取path的绝对路径
     */
    std::string getAbsolutePath(const std::string& path) const;
    /**
     * brief: 获取配置文件路径
     */
    std::string getConfigPath();

private:
    // RWMutex
    RWMutexType m_mutex;
    // 传入参数列表
    std::map<std::string, std::string> m_args;
    // 帮助信息列表
    std::vector<std::pair<std::string, std::string> > m_helps;
    // 可执行程序名
    std::string m_program;
    // 当前进程运行目录
    std::string m_cwd;
    // 当前进程可执行文件的完整路径
    std::string m_exe;
};

typedef Singleton<Enviroment> EnvMgr;


}

#endif /*__BIFANG_ENVIROMENT_H*/
