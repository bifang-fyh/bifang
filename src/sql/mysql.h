/**
 * brief: MySQL封装
 */
#ifndef __BIFANG_MYSQL_H
#define __BIFANG_MYSQL_H

#include <functional>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <string.h>
#include <time.h>

#include <mysql/mysql.h>
#include "singleton.h"
#include "util.h"
#include "version.h"
#include "lock.h"
#include "config.h"


#ifdef MYSQL_ENABLE

namespace bifang
{

namespace mysql
{
class MySQLStmt;
class MySQL;
class MySQLTransaction;
class MySQLManager;

struct MySQLTime
{
    MySQLTime(time_t t)
        :ts(t)
    {
    }
    time_t ts;
};

struct MySQLNull
{
};


/**
 * brief: 将MYSQL_BIND的部分信息打印出来(类型, *有无符号, 数据长度, 数据)
 */
std::string mysql_bind_to_string(const MYSQL_BIND& bind);

/**
 * brief: 将MYSQL_TIME转换为时间戳
 */
time_t mysql_time_to_time_t(const MYSQL_TIME& mt);

/**
 * brief: 将时间戳转换为MYSQL_TIME
 */
MYSQL_TIME time_t_to_mysql_time(const time_t& ts);


/**
 * brief: MySQL查询结果集类
 */
class MySQLRes
{
public:
    typedef std::shared_ptr<MySQLRes> ptr;

    MySQLRes(MYSQL_RES* res)
        :m_cur(nullptr)
        ,m_curLength(nullptr)
    {
        m_data.reset(res, mysql_free_result);
    }

    /**
     * brief: 将结果集移向下一行
     * return: true  - 成功
     *         false - 失败
     */
    bool next()
    {
        m_cur = mysql_fetch_row(m_data.get());
        m_curLength = mysql_fetch_lengths(m_data.get());
        return !!m_cur;
    }

    /**
     * brief: 获取原始结果集
     */
    MYSQL_RES* get() const { return m_data.get(); }

    /**
     * brief: 获取结果集的行数
     */
    int getRows() const { return mysql_num_rows(m_data.get()); }

    /**
     * brief: 获取结果集每一列的名称
     * param: v 查询结果字段列表
     */
    void getFieldList(std::vector<std::string>& v)
    {
        int len = mysql_num_fields(m_data.get());
        MYSQL_FIELD* fields = mysql_fetch_fields(m_data.get());
        for (int i = 0; i < len; i++)
            v.push_back(std::string(fields[i].name, fields[i].name_length));
    }

    /**
     * brief: 获取结果集当前行的所有数据(string)
     * param: v 查询结果列表
     */
    void getResList(std::vector<std::string>& v)
    {
        int len = mysql_num_fields(m_data.get());
        for (int i = 0; i < len; i++)
            v.push_back(std::string(m_cur[i], m_curLength[i]));
    }

public:
    /**
     * brief: 获取结果集指定列的数据
     */
    bool isNull(int idx)        const { return m_cur[idx] ? false : true; }
    int8_t getInt8(int idx)     const { return getInt64(idx);             }
    uint8_t getUint8(int idx)   const { return getInt64(idx);             }
    int16_t getInt16(int idx)   const { return getInt64(idx);             }
    uint16_t getUint16(int idx) const { return getInt64(idx);             }
    int32_t getInt32(int idx)   const { return getInt64(idx);             }
    uint32_t getUint32(int idx) const { return getInt64(idx);             }
    int64_t getInt64(int idx)   const { return atoll(m_cur[idx]);         }
    uint64_t getUint64(int idx) const { return getInt64(idx);             }
    float getFloat(int idx)     const { return getDouble(idx);            }
    double getDouble(int idx)   const { return atof(m_cur[idx]);          }
    std::string getString(int idx) const { return std::string(m_cur[idx], m_curLength[idx]); }
    std::string getBlob(int idx)   const { return std::string(m_cur[idx], m_curLength[idx]); }
    time_t getTime(int idx)      const { return m_cur[idx] ? string_to_time(m_cur[idx]) : 0; }

private:
    // 数据集当前指向的行数据(MYSQL_ROW = char**)
    MYSQL_ROW m_cur;
    // 当前行每一列的数据长度
    unsigned long* m_curLength;
    // MYSQL结果集智能指针
    std::shared_ptr<MYSQL_RES> m_data;
};

/**
 * brief: MySQL预处理查询结果集类(由于需要事先分配储存结果的内存, 所以需要在建表时需要确认数据长度(int, float之类的数据不需要), 不然无法使用该功能)
 */
class MySQLStmtRes
{
public:
    typedef std::shared_ptr<MySQLStmtRes> ptr;

    static MySQLStmtRes::ptr create(std::shared_ptr<MySQLStmt> stmt);

    MySQLStmtRes(std::shared_ptr<MySQLStmt> stmt)
        :m_stmt(stmt)
    {
    }

    /**
     * brief: 将结果集移向下一行
     * return: true  - 成功
     *         false - 失败
     */
    bool next();

    /**
     * brief: 获取结果集的行数
     */
    uint64_t getRows();

    /**
     * brief: 获取结果集每一列的名称
     * param: v 查询结果字段列表
     */
    void getFieldList(std::vector<std::string>& v);

    /**
     * brief: 获取结果集当前行的所有数据(string)
     * param: v 查询结果列表
     *        is_convert 是否将时间类型转换为可读性的字符串
     */
    void getResList(std::vector<std::string>& v, bool is_convert = true);

#define XX(type) \
    return *(type*)m_datas[idx].buffer
    bool isNull(int idx)        const { return m_datas[idx].is_null; }
    int8_t getInt8(int idx)     const { XX(int8_t);                  }
    uint8_t getUint8(int idx)   const { XX(uint8_t);                 }
    int16_t getInt16(int idx)   const { XX(int16_t);                 }
    uint16_t getUint16(int idx) const { XX(uint16_t);                }
    int32_t getInt32(int idx)   const { XX(int32_t);                 }
    uint32_t getUint32(int idx) const { XX(uint32_t);                }
    int64_t getInt64(int idx)   const { XX(int64_t);                 }
    uint64_t getUint64(int idx) const { XX(uint64_t);                }
    float getFloat(int idx)     const { XX(float);                   }
    double getDouble(int idx)   const { XX(double);                  }
    std::string getString(int idx) const { return std::string(m_datas[idx].buffer, m_datas[idx].length); }
    std::string getBlob(int idx)   const { return std::string(m_datas[idx].buffer, m_datas[idx].length); }
    time_t getTime(int idx)      const { return mysql_time_to_time_t(*(MYSQL_TIME*)m_datas[idx].buffer); }
#undef XX

private:
    /**
     * brief: 分配用于存储结果集的内存, 实现统一的写入和读取
     */
    struct Data
    {
        ~Data()
        {
            if (buffer)
                delete[] buffer;
        }

        void alloc(size_t size)
        {
            if (buffer)
                delete[] buffer;

            buffer = new char[size];
            buffer_length = size;
        }

        uint64_t length = 0;
        bool is_null = false;
        bool error = false;
        char* buffer = nullptr;
        uint64_t buffer_length = 0;
        enum_field_types buffer_type = MYSQL_TYPE_NULL;
    };

private:
    // MySQL预处理类智能指针
    std::shared_ptr<MySQLStmt> m_stmt;
    // 绑定参数(相当于壳)
    std::vector<MYSQL_BIND> m_binds;
    // 绑定参数的内容
    std::vector<Data> m_datas;
};

/**
 * brief: MySQL预处理类
 */
class MySQLStmt : public std::enable_shared_from_this<MySQLStmt>
{
public:
    typedef std::shared_ptr<MySQLStmt> ptr;

    static MySQLStmt::ptr create(std::shared_ptr<MySQL> sql, const std::string& stmt);

    MySQLStmt(std::shared_ptr<MySQL> sql, MYSQL_STMT* stmt)
        :m_mysql(sql)
        ,m_stmt(stmt)
    {
    }

    ~MySQLStmt()
    {
        if (m_stmt)
        {
            mysql_stmt_free_result(m_stmt);
            mysql_stmt_close(m_stmt);
        }

        for (auto& i : m_binds)
        {
            if (i.buffer)
                free(i.buffer);
        }
    }

    uint32_t getErrno() const
    {
        return mysql_stmt_errno(m_stmt);
    }
    std::string getErrstr() const
    {
        const char* errstr = mysql_stmt_error(m_stmt);
        return errstr ? errstr : "";
    }
    /**
     * brief: 返回insert或update语句为AUTO_INCREMENT列生产的值, 在包含AUTO_INCREMENT
     *        字段的表上执行了预处理语句后使用
     * return: 更新或插入字段自动生成的值
     */
    uint64_t getInsertId() const { return mysql_stmt_insert_id(m_stmt); }
    MYSQL_STMT* get() const { return m_stmt; }
    std::vector<MYSQL_BIND>& getBinds() { return m_binds; }

    /**
     * brief: 执行预处理查询(先绑参数后执行)
     * return: true  - 成功
     *         false - 失败
     */
    bool execute();

    /**
     * brief: 执行预处理查询(先绑参数后执行)
     * return: MySQLStmtRes智能指针 - 成功
     *         nullptr - 失败
     */
    MySQLStmtRes::ptr query();

    /**
     * brief: 输出当前绑定参数信息
     */
    std::string toString()
    {
        std::stringstream ss;
        for (auto& i : m_binds)
            ss << mysql_bind_to_string(i) << std::endl;
        return ss.str();
    }

public:
#define BIND_XX(type, symbol, ptr, size) \
    m_binds[idx].buffer_type = type; \
    m_binds[idx].is_unsigned = symbol; \
    if (m_binds[idx].buffer == nullptr) \
    { \
        m_binds[idx].buffer = malloc(size); \
        m_binds[idx].buffer_length = size; \
    } \
    else if (m_binds[idx].buffer_length != size) \
    { \
        free(m_binds[idx].buffer); \
        m_binds[idx].buffer = malloc(size); \
        m_binds[idx].buffer_length = size; \
    } \
    memcpy(m_binds[idx].buffer, ptr, size);

    void bind(int idx, const MySQLNull& value)
    {
        m_binds[idx].buffer_type = MYSQL_TYPE_NULL;
        if (m_binds[idx].buffer != nullptr)
            free(m_binds[idx].buffer);
    }
    void bind(int idx, const int8_t& value)
    {
        BIND_XX(MYSQL_TYPE_TINY, false, &value, sizeof(value));
    }
    void bind(int idx, const uint8_t& value)
    {
        BIND_XX(MYSQL_TYPE_TINY, true, &value, sizeof(value));
    }
    void bind(int idx, const int16_t& value)
    {
        BIND_XX(MYSQL_TYPE_SHORT, false, &value, sizeof(value));
    }
    void bind(int idx, const uint16_t& value)
    {
        BIND_XX(MYSQL_TYPE_SHORT, true, &value, sizeof(value));
    }
    void bind(int idx, const int32_t& value)
    {
        BIND_XX(MYSQL_TYPE_LONG, false, &value, sizeof(value));
    }
    void bind(int idx, const uint32_t& value)
    {
        BIND_XX(MYSQL_TYPE_LONG, true, &value, sizeof(value));
    }
    void bind(int idx, const int64_t& value)
    {
        BIND_XX(MYSQL_TYPE_LONGLONG, false, &value, sizeof(value));
    }
    void bind(int idx, const uint64_t& value)
    {
        BIND_XX(MYSQL_TYPE_LONGLONG, true, &value, sizeof(value));
    }
    void bind(int idx, const float& value)
    {
        BIND_XX(MYSQL_TYPE_FLOAT, false, &value, sizeof(value));
    }
    void bind(int idx, const double& value)
    {
        BIND_XX(MYSQL_TYPE_DOUBLE, false, &value, sizeof(value));
    }
    void bind(int idx, const std::string& value)
    {
        BIND_XX(MYSQL_TYPE_STRING, false, value.c_str(), value.size());
    }
    //
    void bind(int idx, const void* value, uint32_t size)
    {
        BIND_XX(MYSQL_TYPE_BLOB, false, value, size);
    }
    void bind(int idx, const void* value, uint64_t size)
    {
        BIND_XX(MYSQL_TYPE_BLOB, false, value, size);
    }
    //
    void bind(int idx, const MySQLTime& value)
    {
        MYSQL_TIME mt = time_t_to_mysql_time(value.ts);
        BIND_XX(MYSQL_TYPE_TIMESTAMP, false, &mt, sizeof(MYSQL_TIME));
    }

public:
    template<typename... Args>
    void multibind(Args... args)
    {
        binder(0, args...);
    }

    void binder(size_t N)
    {
        //std::cout << "multibind end" << std::endl;
    }

    template<typename... Args>
    void binder(size_t N, const void* value, uint64_t size, Args... args)
    {
        if (N >= m_binds.size())
            return;
        bind(N, value, size);
        binder(N + 1, args...);
    }

    template<typename T, typename... Args>
    void binder(size_t N, T value, Args... args)
    {
        if (N >= m_binds.size())
            return;
        bind(N, value);
        binder(N + 1, args...);
    }

private:
    // 持有MySQL类的智能指针, 防止其提前被释放, 无其他作用
    std::shared_ptr<MySQL> m_mysql;
    // 预处理指令句柄
    MYSQL_STMT* m_stmt;
    // 需绑定的参数
    std::vector<MYSQL_BIND> m_binds;
};

/**
 * brief: MySQL类
 */
class MySQL : public std::enable_shared_from_this<MySQL>
{
friend class MySQLManager;
public:
    typedef std::shared_ptr<MySQL> ptr;

    MySQL(const std::string& host, int port, const std::string& user,
        const std::string& passwd, const std::string& dbname,
        uint32_t poolSize = 10);

public:
    uint32_t getErrno() const
    {
        if (!m_mysql)
            return -1;
        return mysql_errno(m_mysql.get());
    }
    std::string getErrstr() const
    {
        if (!m_mysql)
            return "mysql is not init";
        const char* str = mysql_error(m_mysql.get());
        return str ? str : "";
    }

    uint64_t getInsertId() const
    {
        if (!m_mysql)
            return 0;
        return mysql_insert_id(m_mysql.get());
    }

    uint64_t getAffectedRows() const
    {
        if (!m_mysql)
            return 0;
        return mysql_affected_rows(m_mysql.get());
    }

    std::shared_ptr<MYSQL> get() const { return m_mysql; }

    bool needToCheck() const
    {
        return !((time(0) - m_lastUsedTime) < 5 && !m_hasError);
    }

public:
    /**
     * brief: 连接数据库
     * return: true  - 成功
     *         false - 失败
     */
    bool connect();

    /**
     * brief: ping数据库
     * return: true  - 成功
     *         false - 失败
     */
    bool ping();

    /**
     * brief: 切换数据库
     * param: dbname 需切换的数据库名
     * return: true  - 成功
     *         false - 失败
     */
    bool use(const std::string& dbname);

    bool execute(const char* format, ...);
    bool execute(const std::string& cmd);

    MySQLRes::ptr query(const char* format, ...);
    MySQLRes::ptr query(const std::string& cmd);

    /**
     * brief: 创建一个当前sql的预处理
     * param: cmd 预处理指令
     * return: MySQLTransaction智能指针 - 成功
     *         nullptr - 失败
     */
    MySQLStmt::ptr openPrepare(const std::string& cmd);
    /**
     * brief: 创建一个当前sql的事务
     * param: auto_commit 是否自动提交事务
     * return: MySQLTransaction智能指针 - 成功
     *         nullptr - 失败
     */
    std::shared_ptr<MySQLTransaction> openTransaction(bool auto_commit);

private:
    // 连接信息
    // 主机地址
    std::string m_host;
    // 主机端口
    int m_port;
    // 用户名
    std::string m_user;
    // 密码
    std::string m_passwd;
    // 数据库名
    std::string m_dbname;
    // 当前mysql句柄
    std::shared_ptr<MYSQL> m_mysql;
    // 最后使用时间
    uint64_t m_lastUsedTime = 0;
    // 是否有错误
    bool m_hasError = false;
    uint32_t m_poolSize;
};

/**
 * brief: MySQL事务类
 */
class MySQLTransaction
{
public:
    typedef std::shared_ptr<MySQLTransaction> ptr;

    static MySQLTransaction::ptr create(MySQL::ptr mysql, bool auto_commit);

    MySQLTransaction(MySQL::ptr mysql, bool auto_commit)
        :m_mysql(mysql)
        ,m_autoCommit(auto_commit)
        ,m_isFinished(false)
        ,m_hasError(false)
    {
    }

    ~MySQLTransaction()
    {
        if (m_autoCommit)
            commit();
        else
            rollback();
    }

    bool isAutoCommit() const { return m_autoCommit; }
    bool isFinished()   const { return m_isFinished; }
    bool isError()      const { return m_hasError;   }

    int64_t getInsertId() const { return m_mysql->getInsertId(); }
    MySQL::ptr getMySQL() const { return m_mysql; }

    bool begin()
    {
        return execute("BEGIN");
    }
    bool commit()
    {
        if (m_isFinished || m_hasError)
            return !m_hasError;

        bool ret = execute("COMMIT");
        if (ret)
            m_isFinished = true;
        else
            m_hasError = true;
        return ret;
    }
    bool rollback()
    {
        if (m_isFinished)
            return true;

        int ret = execute("ROLLBACK");
        if (ret)
            m_isFinished = true;
        else
            m_hasError = true;
        return ret;
    }

    bool execute(const char* format, ...);
    bool execute(const std::string& cmd);

private:
    // 持有MySQL类的智能指针
    MySQL::ptr m_mysql;
    // 是否自动提交
    bool m_autoCommit;
    // 是否已执行完成
    bool m_isFinished;
    // 是否有错误
    bool m_hasError;
};

/**
 * brief: MySQL管理类
 */
class MySQLManager
{
public:
    typedef Mutex MutexType;

    MySQLManager();

    ~MySQLManager();

    bool add(const std::string& name, const std::string& host, int port,
        const std::string& user, const std::string& passwd,
        const std::string& dbname, uint32_t poolSize = 10);

    MySQL::ptr get(const std::string& name);

    bool execute(const std::string& name, const char* format, ...);
    bool execute(const std::string& name, const std::string& cmd);

    MySQLRes::ptr query(const std::string& name, const char* format, ...);
    MySQLRes::ptr query(const std::string& name, const std::string& cmd);

    MySQLStmt::ptr openPrepare(const std::string& name, const std::string& cmd);

    MySQLTransaction::ptr openTransaction(const std::string& name, bool auto_commit);

    void checkConnection(int sec = 30);

private:
    void freeMySQL(const std::string& name, MySQL* m);

private:
    // Mutex
    MutexType m_mutex;
    std::unordered_map<std::string, std::list<MySQL*> > m_connections;
    std::unordered_map<std::string, MySqlConf> m_sqlDefines;
};

typedef bifang::Singleton<MySQLManager> MySQLMgr;

}

namespace MySQLUtil
{
    bool execute(const std::string& name, const char* format, ...);
    bool execute(const std::string& name, const std::string& cmd);

    mysql::MySQLRes::ptr query(const std::string& name, const char* format, ...);
    mysql::MySQLRes::ptr query(const std::string& name, const std::string& cmd);

    mysql::MySQLStmt::ptr openPrepare(const std::string& name, const std::string& cmd);

    mysql::MySQLTransaction::ptr openTransaction(const std::string& name, bool auto_commit);

    void checkConnection(int sec = 30);
}

}

#endif

#endif /*__BIFANG_MYSQL_H*/

