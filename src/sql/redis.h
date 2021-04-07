/**
 * brief: Redis封装
 */
#ifndef __BIFANG_REDIS_H
#define __BIFANG_REDIS_H

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

#include <hiredis/hiredis.h>
#include "singleton.h"
#include "util.h"
#include "version.h"
#include "lock.h"
#include "config.h"


#ifdef REDIS_ENABLE

namespace bifang
{

namespace redis
{

typedef std::shared_ptr<redisReply> ReplyPtr;

enum RedisType
{
    RedisNone = 0,
    RedisString,
    RedisList,
    RedisSet,
    RedisZset,
    RedisHash,
};

std::string TypetoString(RedisType type);

/**
 * brief: Redis类
 */
class Redis
{
public:
    typedef std::shared_ptr<Redis> ptr;

    Redis(const std::string& host, int port, const std::string& passwd,
        uint32_t connectTimeout = 100, uint32_t cmdTimeout = 100,
        uint32_t poolSize = 10);

    bool reconnect();
    bool connect();

    ReplyPtr cmd(const char* format, ...);
    ReplyPtr cmd(const std::vector<std::string>& argv);

    bool appendCmd(const char* format, ...);
    bool appendCmd(const std::vector<std::string>& argv);

    uint64_t getLastUsedTime() const { return m_lastUsedTime; }
    uint32_t getPoolSize() const     { return m_poolSize;     }
    bool needToCheck() const { return time(0) - m_lastUsedTime > 30; }

    void setLastUsedTime(uint64_t v) { m_lastUsedTime = v; }

public:
    /**
     * brief: 删除key
     * param: keys 需删除key的列表
     * return: -1   - 失败
     *         其他 - 删除成功个数
     */
    int64_t del(std::unordered_set<std::string> keys);

    /**
     * brief: 返回key所储存的值的类型
     * param: key 需查询的key
     * return: RedisNone(key不存在), RedisString(字符串), RedisList(列表),
     *         RedisSet(集合), RedisZset(有序集), RedisHash(哈希表)
     */
    RedisType type(const std::string& key);

    /**
     * brief: 获取key列表
     * param: pattern 查询的规则
     * return: key列表
     */
    std::unordered_set<std::string> keys(const std::string& pattern);

    /**
     * brief: 查看key的剩余生存时间
     * param: key 需查询的key
     * return: -3   - 查询出错
     *         -2   - key不存在
     *         -1   - key无设置生存时间
     *         其他 - key的生存时间
     */
    int64_t ttl(const std::string& key);

    /**
     * brief: 查看key是否存在
     * param: key 需查询的key
     * return: true  - 存在
     *         false - 不存在
     */
    bool exists(const std::string& key);

    /**
     * brief: 设置key的生存时间, 过期删除
     * param: key 需设置的key
     *        seconds 生存时间(秒)
     * return: true  - 设置成功
     *         false - 设置失败(可能是key不存在或者其他原因)
     */
    bool expire(const std::string& key, int64_t seconds);

    /**
     * brief: 移除给定key的生存时间, 将这个key从可挥发的(带生存时间key)
     *        转换成持久化的(一个不带生存时间, 永不过期的key)
     * param: key 需设置的key
     * return: true  - 设置成功
     *         false - 设置失败(可能是key不存在, key本身没有设置生存时间或者其他原因)
     */
    bool persist(const std::string& key);

/*----------string----------*/
    /**
     * brief: 设置key的值, 如果key已经持有其他值, 就覆写旧值, 无视类型
     * param: key 需设置的key
     *        value 需写入的值
     * return: true  - 写入成功
     *         false - 写入失败
     */
    bool set(const std::string& key, const std::string& value);

    /**
     * brief: 设置key的值, 若给定的key已经存在, 则不做任何动作
     * param: key 需设置的key
     *        value 需写入的值
     * return: true  - 写入成功
     *         false - 写入失败(值已存在或者其他错误)
     */
    bool setnx(const std::string& key, const std::string& value);

    /**
     * brief: 将设置某个key的值, 并将key的生存时间设为seconds
     * param: key 需设置的key
     *        seconds 生存时间(秒)
     *        value 需设置的值
     * return: true  - 写入成功
     *         false - 写入失败
     */
    bool setex(const std::string& key, int64_t seconds, const std::string& value);

    /**
     * brief: 返回key所关联的字符串值
     * param: key 需查询的key
     * return: key对应的值(获取失败返回空串, 失败原因可能是key的类型不为string或者其他原因)
     */
    std::string get(const std::string& key);

    /**
     * brief: 将key中储存的数字值增一
     *        (如果key不存在, 那么key的值会先被初始化为0, 然后再执行INCR操作)
     * param: key 需设置的key
     * return: true  - 成功
     *         false - 失败(值包含错误的类型, 或字符串类型的值不能表示为数字)
     */
    bool incr(const std::string& key);

    /**
     * brief: 将key中储存的数字值增加固定值increment
     *        (如果key不存在, 那么key的值会先被初始化为0, 然后再执行INCRBY操作)
     * param: key 需设置的key
     *        increment 增加的固定值
     * return: true  - 成功
     *         false - 失败(值包含错误的类型, 或字符串类型的值不能表示为数字)
     */
    bool incrBy(const std::string& key, int64_t increment);

    /**
     * brief: 将key中储存的数字值减一
     *        (如果key不存在, 那么key的值会先被初始化为0, 然后再执行DECR操作)
     * param: key 需设置的key
     * return: true  - 成功
     *         false - 失败(值包含错误的类型, 或字符串类型的值不能表示为数字)
     */
    bool decr(const std::string& key);

    /**
     * brief: 将key中储存的数字值减少固定值decrement
     *        (如果key不存在, 那么key的值会先被初始化为0, 然后再执行DECRBY操作)
     * param: key 需设置的key
     *        decrement 减少的固定值
     * return: true  - 成功
     *         false - 失败(值包含错误的类型, 或字符串类型的值不能表示为数字)
     */
    bool decrBy(const std::string& key, int64_t decrement);
/*----------string----------*/

/*----------hash----------*/
    /**
     * brief: 将哈希表key中field字段的值设置为value
     * param: key 需设置的key
     *        field 需设置的字段
     *        value 需设置的值
     * return: true  - 成功
     *         false - 失败
     */
    bool hset(const std::string& key, const std::string& field, const std::string& value);

    /**
     * brief: 当且仅当域field不存在, 将哈希表key中field字段的值设置为value
     * param: key 需设置的key
     *        field 需设置的字段
     *        value 需设置的值
     * return: true  - 成功
     *         false - 失败
     */
    bool hsetnx(const std::string& key, const std::string& field, const std::string& value);

    /**
     * brief: 同时将多个field-value(域-值)对设置到哈希表key中, 此命令会覆盖哈希表
     *        中已存在的域, 如果key不存在, 将会创建一个空哈希表并执行HMSET操作
     * param: key 需设置的key
     *        hash 域-值对列表
     * return: true  - 成功
     *         false - 失败
     */
    bool hmset(const std::string& key, const std::unordered_map<std::string, std::string>& hashs);

    /**
     * brief: 返回哈希表key中给定field字段的值
     * param: key 需查询的key
     *        field 需查询的字段
     * return: 查询字段的值, 获取失败或者字段不存在均返回空字符串
     */
    std::string hget(const std::string& key, const std::string& field);

    /**
     * brief: 返回哈希表中所有的字段和对应的值
     * param: key 需查询的key
     * return: 返回key对应的整个hash table
     */
    std::unordered_map<std::string, std::string> hgetAll(const std::string& key);

    /**
     * brief: 删除哈希表中的一个字段, 字段不存在将被忽略
     * param: key 需设置的key
     *        fields 需删除字段列表
     * return: -1   - 类型不正确或者其他错误
     *         其他 - 被成功移除的字段的数量, 不包括被忽略的域
     */
    int64_t hdel(const std::string& key, std::initializer_list<std::string> fields);

    /**
     * brief: 返回哈希表中字段的数量
     * param: key 需查询的key
     * return: -1   - 类型不正确或者其他错误
     *         其他 - 哈希表中字段的数量(当key不存在时, 返回0)
     */
    int64_t hlen(const std::string& key);

    /**
     * brief: 查看哈希表key中指定的field字段是否存在
     * param: key 需查询的key
     *        field 需查询的字段
     * return: true  - 存在
     *         false - 不存在
     */
    bool hexists(const std::string& key, const std::string& field);

    /**
     * brief: 为哈希表key中field字段的值加上增量increment
     * param: key 需设置的key
     *        field 需设置的字段
     *        increment 增加的固定值
     * return: true  - 成功
     *         false - 失败(值包含错误的类型, 或字符串类型的值不能表示为数字)
     */
    bool hincrBy(const std::string& key, const std::string& field, int64_t increment);

    /**
     * brief: 返回哈希表key中的所有字段
     * param: key 需查询的key
     * return: key中包含的字段列表
     */
    std::unordered_set<std::string> hkeys(const std::string& key);

    /**
     * brief: 返回哈希表key中所有字段的值
     * param: key 需查询的key
     * return: key中包含字段的值列表
     */
    std::unordered_set<std::string> hvals(const std::string& key);
/*----------hash----------*/

/*----------list----------*/
    /**
     * brief: 将一个值value插入到列表key的表头
     * param: key 需设置的key
     *        values 需插入的列表
     * return: -1   - 失败
     *         其他 - 执行LPUSH命令后, 列表的长度
     */
    int64_t lpush(const std::string& key, std::vector<std::string> values);
    /**
     * brief: 将一个值value插入到列表key的表尾(最右边)
     * param: key 需设置的key
     *        values 需插入的列表
     * return: -1   - 失败
     *         其他 - 执行RPUSH命令后, 列表的长度
     */
    int64_t rpush(const std::string& key, std::vector<std::string> values);

    /**
     * brief: 移除并返回列表key的头元素(count个)
     * param: key 需设置的key
     *        count 移除的个数
     * return: 移除的头元素组成的列表
     */
    std::vector<std::string> lpop(const std::string& key, int64_t count = 1);
    /**
     * brief: 移除并返回列表key的尾元素(count个)
     * param: key 需设置的key
     *        count 移除的个数
     * return: 移除的尾元素组成的列表
     */
    std::vector<std::string> rpop(const std::string& key, int64_t count = 1);

    /**
     * brief: 返回列表的长度
     * param: key 需查询的key
     * return: -1   - 类型不正确或其他错误
     *         其他 - 列表的长度
     */
    int64_t llen(const std::string& key);

    /**
     * brief: 返回列表中指定区间内的元素, 区间以偏移量start和end指定, 也可以使用
     *        负数下标，以-1表示列表的最后一个元素, -2表示列表的倒数第二个元素, 以此类推
     * param: key 需查询的key
     *        start 查询起始位置
     *        end 查询终止位置
     * return: 范围对应的元素列表
     */
    std::vector<std::string> lrange(const std::string& key, int64_t start, int64_t end);

    /**
     * brief: 根据参数count的值, 移除列表中与参数value相等的元素
     *        count = 0 - 移除表中所有与value相等的值
     *        count > 0 - 从表头开始向表尾搜索, 移除与value相等的元素, 数量为count
     *        count < 0 - 从表尾开始向表头搜索, 移除与value相等的元素, 数量为count的绝对值
     * param: key 需设置的key
     *        count 判断参数
     *        value 需移除的元素
     * return: -1   - 类型不正确或其他错误
     *         其他 - 被移除元素的数量
     */
    int64_t lrem(const std::string& key, int64_t count, const std::string& value);
/*----------list----------*/

/*----------set----------*/
    /**
     * brief: 将列表key下标为index的元素的值设置为value
     * param: key 需设置的key
     *        index 元素下标
     *        value 修改后的值
     * return: true  - 修改成功
     *         false - 修改失败(类型不正确或者其他原因)
     */
    bool lset(const std::string& key, int64_t index, const std::string& value);

    /**
     * brief: 将一个或多个member元素加入到集合key当中, 已经存在于集合的member的
     *        元素将被忽略, 假如key不存在, 则创建一个只包含member元素作成员的集合
     * param: key 需设置的key
     *        members 元素集合
     * return: -1   - 类型不正确或其他错误
     *         其他 - 插入成功个数
     */
    int64_t sadd(const std::string& key, std::initializer_list<std::string> members);

    /**
     * brief: 移除集合key中的一个或多个member元素, 不存在的member元素会被忽略
     * param: key 需设置的key
     *        members 元素集合
     * return: -1   - 类型不正确或其他错误
     *         其他 - 被成功移除的元素的数量, 不包括被忽略的元素
     */
    int64_t srem(const std::string& key, std::initializer_list<std::string> members);

    /**
     * brief: 返回集合key中的所有成员, 不存在的key被视为空集合
     * param: key 需查询的key
     * return: 集合中所有成员的列表(出现错误也是返回空列表)
     */
    std::unordered_set<std::string> smembers(const std::string& key);

    /**
     * brief: 判断member元素是不是集合key的成员
     * param: key 需查询的key
     *        member 需查询的元素
     * return: true  - member是key的成员
     *         false - member不是key的成员, 或者key类型不正确
     */
    bool sismember(const std::string& key, const std::string& member);

    /**
     * brief: 返回集合key中元素的数量(集合中元素的数量)
     * param: key 需查询的key
     * return: -1   - 类型不正确或其他错误
     *         其他 - 集合key中元素的数量
     */
    int64_t scard(const std::string& key);

    /**
     * brief: 移除并返回集合key中的count元素
     * param: key 需设置的key
     *        count 移除元素的个数
     * return: 空串 - 类型不正确或其他错误
     *         其他 - 被移除的元素列表
     */
    std::unordered_set<std::string> spop(const std::string& key, int64_t count = 1);

    /**
     * brief: 返回一个集合的全部成员, 该集合是所有给定集合的交集
     * param: keys 需查找交集的key列表
     * return: 交集成员的列表, 出错或者没有交集则返回空集
     */
    std::unordered_set<std::string> sinter(std::initializer_list<std::string> keys);

    /**
     * brief: 返回一个集合的全部成员，该集合是所有给定集合的并集
     * param: keys 需查找并集的key列表
     * return: 并集成员的列表, 出错或者没有并集则返回空集
     */
    std::unordered_set<std::string> sunion(std::initializer_list<std::string> keys);

    /**
     * brief: 返回一个集合的全部成员, 该集合是所有给定集合之间的差集
     * param: keys 需查找差集的key列表
     * return: 差集成员的列表, 出错或者没有差集则返回空集
     */
    std::unordered_set<std::string> sdiff(std::initializer_list<std::string> keys);
/*----------set----------*/

/*----------zset----------*/
    /**
     * brief: 将一个member元素及其score值加入到有序集key当中
     * param: key 需设置的key
     *        score_members 需插入的score和member组成的对的列表
     * return: -1   - 类型不正确或其他错误
     *         其他 - 被成功添加的新成员的数量, 不包括那些被更新的或已经存在的成员
     */
    int64_t zadd(const std::string& key, std::initializer_list<std::pair<double, std::string>> score_members);

    /**
     * brief: 移除集合key中的一个或多个member元素, 不存在的member元素会被忽略
     * param: key 需设置的key
     *        member 需插入的score和member组成的对的列表
     * return: -1   - 类型不正确或其他错误
     *         其他 - 被成功移除元素的数量, 不包括被忽略的元素
     */
    int64_t zrem(const std::string& key, std::initializer_list<std::string> members);

    /**
     * brief: 返回有序集key中元素的个数
     * param: key 需查询的key
     * return: -1   - 类型不正确或其他错误
     *         其他 - 返回有序集key中元素的个数(当key不存在时, 返回0)
     */
    int64_t zcard(const std::string& key);

    /**
     * brief: 返回有序集key中, score值在min和max之间(默认包括score值等于min或max)的成员的个数
     * param: key 需查询的key
     *        min score最小值
     *        max score最大值
     * return: -1   - 类型不正确或其他错误
     *         其他 - score值在min和max之间的成员的数量
     */
    int64_t zcount(const std::string& key, double min, double max);

    /**
     * brief: 返回有序集key中, 成员member的score值
     * param: key 需查询的key
     *        member 需查询的成员
     * return: -1   - 类型不正确, meber不存在或其他错误
     *         其他 - member的score值
     */
    double zscore(const std::string& key, const std::string& member);

    /**
     * brief: 为有序集key的成员member的score值加上增量increment
     * param: key 需设置的key
     *        increment score增量
     *        member 需查询的成员
     * return: -1   - 其他错误
     *         其他 - member成员的新score值
     */
    double zincrby(const std::string& key, double increment, const std::string& member);

    /**
     * brief: 返回有序集key中, 指定区间内的成员, 其中成员的位置按score值递增(从小到大)来排序,
     *        你也可以使用负数下标, 以-1表示最后一个成员, -2表示倒数第二个成员, 以此类推
     * param: key 需查询的key
     *        start 查询起始位置
     *        end 查询终止位置
     * return: 指定区间内, 有序集成员的列表
     */
    std::vector<std::string> zrange(const std::string& key, int64_t start, int64_t end);

    /**
     * brief: 返回有序集key中, 指定区间内的成员, 其中成员的位置按score值递减(从大到小)来排序,
     *        你也可以使用负数下标, 以-1表示最后一个成员, -2表示倒数第二个成员, 以此类推
     * param: key 需查询的key
     *        start 查询起始位置
     *        end 查询终止位置
     * return: 指定区间内, 有序集成员的列表
     */
    std::vector<std::string> zrevrange(const std::string& key, int64_t start, int64_t end);

    /**
     * brief: 返回有序集key中, 所有score值介于min和max之间(包括等于min或max)的成员,
     *        有序集成员按score值递增(从小到大)次序排列
     * param: key 需查询的key
     *        min score最小值
     *        max score最大值
     * return: 指定区间内, 有序集成员的列表
     */
    std::vector<std::string> zrangeByScore(const std::string& key, double min, double max);

    /**
     * brief: 返回有序集key中成员member的排名, 其中有序集成员按score值递增(从小到大)顺序排列
     * param: key 需查询的key
     *        member 需查询的成员
     * return: -1   - 类型不正确或者member不是key的成员或者其他错误
     *         其他 - member的排名
     */
    int64_t zrank(const std::string& key, const std::string& member);

    /**
     * brief: 返回有序集key中成员member的排名, 其中有序集成员按score值递减(从大到小)排序
     * param: key 需查询的key
     *        member 需查询的成员
     * return: -1   - 类型不正确或者member不是key的成员或者其他错误
     *         其他 - member的排名
     */
    int64_t zrevrank(const std::string& key, const std::string& member);

    /**
     * brief: 移除有序集key中, 所有score值介于min和max之间(包括等于min或max)的成员
     * param: key 需设置的key
     *        min score最小值
     *        max score最大值
     * return: -1   - 类型不正确或者其他错误
     *         其他 - 被移除成员的数量
     */
    int64_t zremrangeByScore(const std::string& key, double min, double max);
/*----------zset----------*/

private:
    // 连接信息
    // 主机地址
    std::string m_host;
    // 主机端口
    int m_port;
    // 密码
    std::string m_passwd;
    // 连接超时时间
    struct timeval m_connectTimeout;
    // 指令超时时间
    struct timeval m_cmdTimeout;
    // 最大连接数
    uint32_t m_poolSize;
    // 最后活跃时间点
    uint64_t m_lastUsedTime;
    std::shared_ptr<redisContext> m_context;
};

/**
 * brief: Redis管理类
 */
class RedisManager
{
public:
    typedef Mutex MutexType;

    ~RedisManager();

    void add(const std::string& name, const std::string& host,
        int port, const std::string& passwd, uint32_t connectTimeout,
        uint32_t cmdTimeout, uint32_t poolSize = 10);

    Redis::ptr get(const std::string& name);

    void checkConnection(int sec = 30);

private:
    void freeRedis(const std::string& name, Redis* r);

private:
    // Mutex
    MutexType m_mutex;
    std::unordered_map<std::string, std::list<Redis*> > m_connections;
    std::unordered_map<std::string, RedisConf> m_sqlDefines;
};

typedef bifang::Singleton<RedisManager> RedisMgr;

}

}

#endif /*REDIS_ENABLE*/

#endif /*__BIFANG_REDIS_H*/
