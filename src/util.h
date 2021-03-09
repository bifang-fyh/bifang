/**
 * brief: 部分通用函数
 */
#ifndef __BIFANG_UTIL_H
#define __BIFANG_UTIL_H

#include <memory>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>
#include <dlfcn.h>
#include <ifaddrs.h>
#include <boost/lexical_cast.hpp>
#include <openssl/sha.h>

#include "json/json.h"
#include "version.h"

namespace bifang
{

/**
 * brief: 打印错误信息
 */
void printError(const std::string& s);

/**
 * brief: 获取当前时间的毫秒
 */
uint64_t getCurrentMS();

/**
 * brief: 获取当前时间的微秒
 */
uint64_t getCurrentUS();

/**
 * brief: 将时间转换为格式化字符串
 * param: ts 需要转换的原始时间
 *        format 转换格式
 * return: 格式化后的时间
 */
std::string time_to_string(time_t ts = time(0),
                const std::string& format = "%Y-%m-%d %H:%M:%S", bool is_gmt = false);

/**
 * brief: 将格式化字符串转换为时间
 */
time_t string_to_time(const std::string& str, const std::string& format = "%Y-%m-%d %H:%M:%S");

/**
 * brief: 将此时间差转换为标准格式
 */
std::string time_difference(time_t ts);

/**
 * brief: 随机数播种
 */
int Srand();

/**
 * brief: 取出类型名称
 */
template<class T>
const char* type_to_name()
{
    const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr,
        nullptr, nullptr);
    return s_name;
}

/**
 * brief: 获取当前机器主机名称
 */
std::string getHostName();
/**
 * brief: 获取当前机器IPv4地址
 */
std::string getIPv4();

/**
 * brief: 获取Map中的key值, 并转成对应类型
 * param: m Map数据结构
 *        key 关键字
 *        def 默认值
 * return: 如果存在且转换成功返回对应的值, 否则返回默认值
 */
template<class V, class Map, class Key>
V getParamValue(const Map& m, const Key& key, const V& def = V())
{
    auto it = m.find(key);
    if (it == m.end())
        return def;

    try
    {
        return boost::lexical_cast<V>(it->second);
    }
    catch (std::exception& e)
    {
        std::stringstream ss;
        ss << "getParamValue exception " << e.what()
           << ", from type name=" << type_to_name<Map>() << " to find"
           << " type name=" << type_to_name<Key>() << ", and transform to"
           << " type name=" << type_to_name<V>();

        printError(ss.str());
    }
    return def;
}


namespace CryptUtil
{
    /**
     * brief: base64相关
     */
    std::string base64_encode(const std::string &src);
    std::string base64_decode(const std::string &src);

    /**
     * brief: sha1sum相关
     */
    std::string sha1sum(const std::string &data);

    struct MD5_CTX
    {
        uint64_t  bytes;
        uint32_t  a, b, c, d;
        u_char    buffer[64];
    };
    void MD5Init(MD5_CTX* ctx);
    void MD5Update(MD5_CTX* ctx, const void* data, size_t size);
    void MD5Final(MD5_CTX* ctx, unsigned char result[16]);
    /**
     * brief: 将输入字符串转换为16位MD5值(默认生成小写的数值)
     * param: src 需要转换的数据
     *        lower 为true时返回结果为小写, 为false时返回结果为大写
     * return: 转换后的MD5值
     * 
     */
    std::string MD5Create16(const std::string& src, bool lower = true);
    /**
     * brief: 将输入字符串转换为32位MD5值(默认生成小写的数值)
     * param: src 需要转换的数据
     *        lower 为true时返回结果为小写, 为false时返回结果为大写
     * return: 转换后的MD5值
     * 
     */
    std::string MD5Create32(const std::string& src, bool lower = true);

    std::string crypt_apr1(const std::string& key, const std::string& salt);
}


namespace FileUtil
{
    /**
     * brief: 检查filename是否存在, 如果st != NULL则将filename的stat传递出去
     * param: filename 文件名
     *        st 文件stat
     * return: true  - filename存在
     *         false - filename不存在
     */
    bool __lstat(const std::string& filename, struct stat* st = NULL);

    /**
     * brief: 根据dirname创建目录
     * param: dirname 目录名
     *        mode 目录权限
     * return: true  - 创建成功
     *         false - 创建失败
     */
    bool __mkdir(const std::string& dirname, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    /**
     * brief: 创建一个名为dirname的目录, 如果已存在则不创建(可以创建多级目录)
     * param: dirname 目录名
     *        mode 目录权限
     * return: true  - 创建成功
     *         false - 创建失败
     */
    bool Mkdir(const std::string& dirname, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    /**
     * brief: 检查pidfile文件对应的进程是否正在运行
     * param: filename 文件名
     * return: true  - 正在运行
     *         false - 文件错误或者所对应的进程未运行
     */
    bool CheckPidfile(const std::string& filename);

    /**
     * brief: 删除名字为path的文件或目录
     * param: path 目录名或者文件名
     * return: true  - 删除成功(文件删除前就已经不存在也视为成功)
     *         false - 删除失败(权限不足, 打开目录失败等等, 具体原因自己看errno)
     */
    bool Rm(const std::string& path);

    /**
     * brief: 将from剪切为to(若to存在则先将其删除, 删除失败则视为剪切失败)
     * param: from 源文件或目录
     *        to 目标文件或目录
     * return: true  - 剪切成功
     *         false - 剪切失败
     */
    bool Mv(const std::string& from, const std::string& to);

    /**
     * brief: 将from复制到to去(to即使存在也不一定会对其进行整个覆盖)
     * param: from 源文件或目录
     *        to 目标文件或目录
     * return: true  - 复制成功
     *         false - 复制失败
     */
    bool Cp(const std::string& from, const std::string& to);

    /**
     * brief: 将相对路径path转换为绝对路径rpath输出
     * param: path 相对路径
     *        rpath 绝对路径
     * return: true  - 转换成功
     *         false - 转换失败
     */
    bool Realpath(const std::string& path, std::string& rpath);

    /**
     * brief: 创建一个to的符号链接到from, 如果to已存在, 则先将其删除
     * param: from 源文件或目录
     *        to 符号连接文件或目录
     * return: true  - 创建成功
     *         false - 创建失败
     */
    bool Symlink(const std::string& from, const std::string& to);

    /**
     * brief: 删除filename
     * param: filename 删除的文件名
     * return: true  - 删除成功
     *         false - 删除失败
     */
    bool Unlink(const std::string& filename);

    /**
     * brief: 获取filename所在的目录名
     * param: filename 文件名
     * return: filename所在的目录名
     */
    std::string Dirname(const std::string& filename);

    /**
     * brief: 获取filename的文件名
     * param: filename 文件的完整路径
     * return: filename的文件名
     */
    std::string Basename(const std::string& filename);

    /**
     * brief: 获取无任何多余修饰的文件名或目录名(去除名字开头的 ./ 和结尾的 / )
     * param: filename 文件名或目录名
     * return: 转换后的文件名或目录名
     */
    std::string PureDirname(const std::string& filename);

    /**
     * brief: 获取目录中的文件与文件夹信息
     * param: dirname 需要获取信息的目录(必须是目录, 且目录必须以 / 结尾)
     *        info 信息列表
     * return: true  - 获取成功
     *         false - 获取失败
     */
    struct FileInfo
    {
        std::string name;
        time_t mtime;
        time_t ctime;
        off_t size;
    };
    bool GetDirInformation(const std::string& dirname, std::vector<FileInfo>& info);

    /**
     * brief: 以写入模式打开filename
     * param: ofs 文件输出流
     *        filename 文件名
     *        mode 打开模式
     * return: true  - 打开写文件成功
     *         false - 打开写文件失败
     */
    bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
             std::ios_base::openmode mode);

    /**
     * brief: 以读取模式打开filename
     * param: ifs 文件输入流
     *        filename 文件名
     *        mode 打开模式
     * return: true  - 打开读文件成功
     *         false - 打开读文件失败
     */
    bool OpenForRead(std::ifstream& ifs, const std::string& filename,
             std::ios_base::openmode mode);

    /**
     * brief: 将dirname目录下的所有后缀为subfix的文件名全部读出来, 存进files中
     * param: files 文件名列表
     *        dirname 查找目录(不要以 / 结尾)
     *        subfix 文件后缀
     */
    void ListAllFile(std::vector<std::string>& files, const std::string& dirname,
             const std::string& subfix);
}


namespace StringUtil
{
    std::string urlEncode(const std::string& str, bool is_path = false);
    std::string urlDecode(const std::string& str, bool is_path = false);
    std::string escapeHtml(const std::string& str);
    int countCompensation(const std::string& str, int offset = 2);
    std::vector<int> countOriginalLength(const std::string& str,
                         int length, int offset = 2);
    std::string findSubPath(const std::string& path, const std::string& root);

    std::string trim(const std::string& str,
                    const std::string& delimit = " \t\r\n");
    std::string trimLeft(const std::string& str,
                    const std::string& delimit = " \t\r\n");
    std::string trimRight(const std::string& str,
                    const std::string& delimit = " \t\r\n");

    /**
     * brief: 获取字符串分隔后的列表(用单个字符分隔)
     * param: str 待分隔字符串
     *        delim 分隔符
     *        max 最大分隔数
     */
    std::vector<std::string> split(const std::string &str, char delim,
                                 size_t max = ~0ul);

    /**
     * brief: 获取字符串分隔后的列表(用字符串分隔)
     * param: str 待分隔字符串
     *        delims 分隔符字符串
     *        max 最大分隔数
     */
    std::vector<std::string> split(const std::string &str,
                                 const std::string& delims, size_t max = ~0ul);

    /**
     * brief: 随机生成一个长度为len的字符串
     * param: len 生成字符串的长度
     *        chars 随机字符的范围
     */
    std::string randomString(size_t len, const std::string& chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    /**
     * brief: 将字符串转为hex格式
     * param: str 源字符串
     *        is_upper 是否大写
     */
    std::string toHexString(const std::string& str, bool is_upper = true);
}


namespace JsonUtil
{
    std::string getString(const Json::Value& root, const std::string& name,
                    const std::string& default_value = "");

    double getDouble(const Json::Value& root, const std::string& name,
               double default_value = 0);

    int32_t getInt32(const Json::Value& root, const std::string& name,
                int32_t default_value = 0);

    uint32_t getUint32(const Json::Value& root, const std::string& name,
                 uint32_t default_value = 0);

    int64_t getInt64(const Json::Value& root, const std::string& name,
                int64_t default_value = 0);

    uint64_t getUint64(const Json::Value& root, const std::string& name,
                 uint64_t default_value = 0);

    Json::Value fromString(const std::string& v);

    std::string toString(const Json::Value& root);
}


}

#endif /*__BIFANG_UTIL_H*/
