#include "redis.h"


#ifdef REDIS_ENABLE

namespace bifang
{

namespace redis
{

static Config<std::unordered_map<std::string, RedisConf> >::ptr g_redis_config =
    ConfigMgr::GetInstance()->get("redis", std::unordered_map<std::string, RedisConf>(), "redis config");

SystemLogger();

std::string TypetoString(RedisType type)
{
    switch (type)
    {
        case RedisString:
            return "string";
        case RedisList:
            return "list";
        case RedisSet:
            return "set";
        case RedisZset:
            return "zset";
        case RedisHash:
            return "hash";
        default:
            return "none";
    }
}

Redis::Redis(const std::string& host, int port, const std::string& passwd,
           uint32_t connectTimeout, uint32_t cmdTimeout, uint32_t poolSize)
    :m_host(host)
    ,m_port(port)
    ,m_passwd(passwd)
    ,m_poolSize(poolSize)
    ,m_lastUsedTime(0)
{
    m_connectTimeout = {(int)connectTimeout / 1000, (int)connectTimeout % 1000 * 1000};
    m_cmdTimeout = {(int)cmdTimeout / 1000, (int)cmdTimeout % 1000 * 1000};
}

bool Redis::reconnect()
{
    if (!m_context)
        return false;
    if (redisReconnect(m_context.get()))
        return false;

    redisSetTimeout(m_context.get(), m_cmdTimeout);
    if (!m_passwd.empty())
    {
        redisReply* r = (redisReply*)redisCommand(m_context.get(), "auth %s", m_passwd.c_str());
        if (!r)
        {
            log_error << "auth error(" << m_host << ":" << m_port << ")";
            return false;
        }

        if (r->type != REDIS_REPLY_STATUS)
        {
            log_error << "auth reply type error:" << r->type << "(" << m_host << ":" << m_port << ")";
            return false;
        }

        if (!r->str)
        {
            log_error << "auth reply str error:NULL(" << m_host << ":" << m_port << ")";
            return false;
        }

        if (strncasecmp(r->str, "OK", 2))
        {
            log_error << "auth error:" << r->str << "(" << m_host << ":" << m_port << ")";
            return false;
        }
    }
    return true;
}

bool Redis::connect()
{
    redisContext* conn = redisConnectWithTimeout(m_host.c_str(), m_port, m_connectTimeout);
    if (conn)
    {
        m_context.reset(conn, redisFree);
        redisSetTimeout(m_context.get(), m_cmdTimeout);

        if (!m_passwd.empty())
        {
            redisReply* r = (redisReply*)redisCommand(conn, "auth %s", m_passwd.c_str());
            if (!r)
            {
                log_error << "auth error(" << m_host << ":" << m_port << ")";
                return false;
            }

            if (r->type != REDIS_REPLY_STATUS)
            {
                log_error << "auth reply type error:" << r->type << "(" << m_host << ":" << m_port << ")";
                return false;
            }

            if (!r->str)
            {
                log_error << "auth reply str error:NULL(" << m_host << ":" << m_port << ")";
                return false;
            }

            if (strncasecmp(r->str, "OK", 2))
            {
                log_error << "auth error:" << r->str << "(" << m_host << ":" << m_port << ")";
                return false;
            }
        }
        return true;
    }
    return false;
}

ReplyPtr Redis::cmd(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    redisReply* r = (redisReply*)redisvCommand(m_context.get(), format, ap);
    if (!r)
    {
        log_error << "redisvCommand error:(" << format << ")(" << m_host << ":" << m_port << ")";
        va_end(ap);
        return nullptr;
    }
    ReplyPtr ret(r, freeReplyObject);
    if (r->type == REDIS_REPLY_ERROR)
    {
        log_error << "redisvCommand error:(" << format << ")(" << m_host << ":" << m_port
            << ")" + (r->str ? ", errstr:" + std::string(r->str) : "");
        va_end(ap);
        return nullptr;
    }
    va_end(ap);
    return ret;
}

ReplyPtr Redis::cmd(const std::vector<std::string>& argv)
{
    std::vector<const char*> v;
    std::vector<size_t> l;
    for (auto& i : argv)
    {
        v.push_back(i.c_str());
        l.push_back(i.size());
    }

    redisReply* r = (redisReply*)redisCommandArgv(m_context.get(), argv.size(), &v[0], &l[0]);
    if (!r)
    {
        log_error << "redisCommandArgv error:(" << m_host << ":" << m_port << ")";
        return nullptr;
    }
    ReplyPtr ret(r, freeReplyObject);
    if (r->type == REDIS_REPLY_ERROR)
    {
        log_error << "redisCommandArgv error:(" << m_host << ":" << m_port
            << ")" + (r->str ? ", errstr:" + std::string(r->str) : "");
        return nullptr;
    }
    return ret;
}

bool Redis::appendCmd(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = redisvAppendCommand(m_context.get(), format, ap);
    va_end(ap);
    return !ret;
}

bool Redis::appendCmd(const std::vector<std::string>& argv)
{
    std::vector<const char*> v;
    std::vector<size_t> l;
    for (auto& i : argv)
    {
        v.push_back(i.c_str());
        l.push_back(i.size());
    }
    return !redisAppendCommandArgv(m_context.get(), argv.size(), &v[0], &l[0]);
}

int64_t Redis::del(std::unordered_set<std::string> keys)
{
    std::string cmd_str = "DEL";
    for (auto it = keys.begin(); it != keys.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

RedisType Redis::type(const std::string& key)
{
    ReplyPtr reply = cmd("TYPE %s", key.c_str());
    if (!reply || !reply->str)
        return RedisNone;
    else if (!strcasecmp(reply->str, "string"))
        return RedisString;
    else if (!strcasecmp(reply->str, "list"))
        return RedisList;
    else if (!strcasecmp(reply->str, "set"))
        return RedisSet;
    else if (!strcasecmp(reply->str, "zset"))
        return RedisZset;
    else if (!strcasecmp(reply->str, "hash"))
        return RedisHash;
    else
        return RedisNone;
}

std::unordered_set<std::string> Redis::keys(const std::string& pattern)
{
    std::unordered_set<std::string> s;
    ReplyPtr reply = cmd("KEYS %s", pattern.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

int64_t Redis::ttl(const std::string& key)
{
    ReplyPtr reply = cmd("TTL %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -3;
    return reply->integer;
}

bool Redis::exists(const std::string& key)
{
    ReplyPtr reply = cmd("EXISTS %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::expire(const std::string& key, int64_t seconds)
{
    ReplyPtr reply = cmd("EXPIRE %s %ld", key.c_str(), seconds);
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::persist(const std::string& key)
{
    ReplyPtr reply = cmd("PERSIST %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::set(const std::string& key, const std::string& value)
{
    return !!cmd("SET %s %s", key.c_str(), value.c_str());
}

bool Redis::setnx(const std::string& key, const std::string& value)
{
    ReplyPtr reply = cmd("SETNX %s %s", key.c_str(), value.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::setex(const std::string& key, int64_t seconds, const std::string& value)
{
    return !!cmd("SETEX %s %ld %s", key.c_str(), seconds, value.c_str());
}

std::string Redis::get(const std::string& key)
{
    ReplyPtr reply = cmd("GET %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_STRING || !reply->str)
        return "";
    return reply->str;
}

bool Redis::incr(const std::string& key)
{
    return !!cmd("INCR %s", key.c_str());
}

bool Redis::incrBy(const std::string& key, int64_t increment)
{
    return !!cmd("INCRBY %s %ld", key.c_str(), increment);
}

bool Redis::decr(const std::string& key)
{
    return !!cmd("DECR %s", key.c_str());
}

bool Redis::decrBy(const std::string& key, int64_t decrement)
{
    return !!cmd("DECRBY %s %ld", key.c_str(), decrement);
}

bool Redis::hset(const std::string& key, const std::string& field, const std::string& value)
{
    return !!cmd("HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
}

bool Redis::hsetnx(const std::string& key, const std::string& field, const std::string& value)
{
    ReplyPtr reply = cmd("HSETNX %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::hmset(const std::string& key,
                const std::unordered_map<std::string, std::string>& hashs)
{
    std::string cmd_str = "HMSET " + key;
    for (auto it = hashs.begin(); it != hashs.end(); it++)
        cmd_str += " " + it->first + " " + it->second;
    return !!cmd(cmd_str.c_str());
}

std::string Redis::hget(const std::string& key, const std::string& field)
{
    ReplyPtr reply = cmd("HGET %s %s", key.c_str(), field.c_str());
    if (!reply || reply->type != REDIS_REPLY_STRING || !reply->str)
        return "";
    return reply->str;
}

std::unordered_map<std::string, std::string> Redis::hgetAll(const std::string& key)
{
    std::unordered_map<std::string, std::string> m;
    ReplyPtr reply = cmd("HGETALL %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(m);
    for (size_t i = 0; i < reply->elements - 1; i += 2)
    {
        std::string field = std::string(reply->element[i]->str);
        std::string value = std::string(reply->element[i + 1]->str);
        m[field] = value;
    }
    return std::move(m);
}

int64_t Redis::hdel(const std::string& key, std::unordered_set<std::string> fields)
{
    std::string cmd_str = "HDEL " + key;
    for (auto it = fields.begin(); it != fields.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::hlen(const std::string& key)
{
    ReplyPtr reply = cmd("HLEN %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

bool Redis::hexists(const std::string& key, const std::string& field)
{
    ReplyPtr reply = cmd("HEXISTS %s %s", key.c_str(), field.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

bool Redis::hincrBy(const std::string& key, const std::string& field, int64_t increment)
{
    return !!cmd("HINCRBY %s %s %ld", key.c_str(), field.c_str(), increment);
}

std::unordered_set<std::string> Redis::hkeys(const std::string& key)
{
    std::unordered_set<std::string> s;
    ReplyPtr reply = cmd("HKEYS %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

std::unordered_set<std::string> Redis::hvals(const std::string& key)
{
    std::unordered_set<std::string> s;
    ReplyPtr reply = cmd("HVALS %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

int64_t Redis::lpush(const std::string& key, std::vector<std::string> values)
{
    std::string cmd_str = "LPUSH " + key;
    for (auto it = values.begin(); it != values.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::rpush(const std::string& key, std::vector<std::string> values)
{
    std::string cmd_str = "RPUSH " + key;
    for (auto it = values.begin(); it != values.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

std::vector<std::string> Redis::lpop(const std::string& key, int64_t count)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("LPOP %s %ld", key.c_str(), count);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

std::vector<std::string> Redis::rpop(const std::string& key, int64_t count)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("RPOP %s %ld", key.c_str(), count);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

int64_t Redis::llen(const std::string& key)
{
    ReplyPtr reply = cmd("LLEN %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

std::vector<std::string> Redis::lrange(const std::string& key,
                                    int64_t start, int64_t end)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("LRANGE %s %ld %ld", key.c_str(), start, end);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

int64_t Redis::lrem(const std::string& key, int64_t count,
                   const std::string& value)
{
    ReplyPtr reply = cmd("LREM %s %ld %s", key.c_str(), count, value.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

bool Redis::lset(const std::string& key, int64_t index, const std::string& value)
{
    return !!cmd("LREM %s %ld %s", key.c_str(), index, value.c_str());
}

int64_t Redis::sadd(const std::string& key, const std::string& member)
{
    std::string cmd_str = "SADD " + key + " " + member;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::sadd(const std::string& key, std::unordered_set<std::string> members)
{
    std::string cmd_str = "SADD " + key;
    for (auto it = members.begin(); it != members.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::srem(const std::string& key, std::unordered_set<std::string> members)
{
    std::string cmd_str = "SREM " + key;
    for (auto it = members.begin(); it != members.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

std::unordered_set<std::string> Redis::smembers(const std::string& key)
{
    std::unordered_set<std::string> s;
    ReplyPtr reply = cmd("SMEMBERS %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

bool Redis::sismember(const std::string& key, const std::string& member)
{
    ReplyPtr reply = cmd("SISMEMBER %s %s", key.c_str(), member.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return false;
    return (bool)reply->integer;
}

int64_t Redis::scard(const std::string& key)
{
    ReplyPtr reply = cmd("SCARD %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

std::unordered_set<std::string> Redis::spop(const std::string& key, int64_t count)
{
    std::unordered_set<std::string> s;
    ReplyPtr reply = cmd("SPOP %s %ld", key.c_str(), count);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

std::unordered_set<std::string> Redis::sinter(std::unordered_set<std::string> keys)
{
    std::unordered_set<std::string> s;
    std::string cmd_str = "SINTER";
    for (auto it = keys.begin(); it != keys.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

std::unordered_set<std::string> Redis::sunion(std::unordered_set<std::string> keys)
{
    std::unordered_set<std::string> s;
    std::string cmd_str = "SUNION";
    for (auto it = keys.begin(); it != keys.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

std::unordered_set<std::string> Redis::sdiff(std::unordered_set<std::string> keys)
{
    std::unordered_set<std::string> s;
    std::string cmd_str = "SDIFF";
    for (auto it = keys.begin(); it != keys.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(s);
    for (size_t i = 0; i < reply->elements; i++)
        s.insert(reply->element[i]->str);
    return std::move(s);
}

int64_t Redis::zadd(const std::string& key, std::unordered_map<double, std::string> score_members)
{
    std::string cmd_str = "ZADD " + key;
    for (auto it = score_members.begin(); it != score_members.end(); it++)
        cmd_str += " " + std::to_string(it->first) + " " + it->second;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::zrem(const std::string& key, std::unordered_set<std::string> members)
{
    std::string cmd_str = "ZREM " + key;
    for (auto it = members.begin(); it != members.end(); it++)
        cmd_str += " " + *it;
    ReplyPtr reply = cmd(cmd_str.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::zcard(const std::string& key)
{
    ReplyPtr reply = cmd("ZCARD %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::zcount(const std::string& key, double min, double max)
{
    ReplyPtr reply = cmd("ZCOUNT %s %f %f", key.c_str(), min, max);
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

double Redis::zscore(const std::string& key, const std::string& member)
{
    ReplyPtr reply = cmd("ZSCORE %s %s", key.c_str(), member.c_str());
    if (!reply || reply->type != REDIS_REPLY_STRING || !reply->str)
        return -1;
    return atof(reply->str);
}

double Redis::zincrby(const std::string& key, double increment, const std::string& member)
{
    ReplyPtr reply = cmd("ZINCRBY %s %f %s", key.c_str(), increment, member.c_str());
    if (!reply || reply->type != REDIS_REPLY_STRING || !reply->str)
        return -1;
    return atof(reply->str);
}

std::vector<std::string> Redis::zrange(const std::string& key,
                                    int64_t start, int64_t end)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("ZRANGE %s %ld %ld", key.c_str(), start, end);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

std::vector<std::string> Redis::zrevrange(const std::string& key,
                                    int64_t start, int64_t end)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("ZREVRANGE %s %ld %ld", key.c_str(), start, end);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

std::vector<std::string> Redis::zrangeByScore(const std::string& key,
                                    double min, double max)
{
    std::vector<std::string> vec;
    ReplyPtr reply = cmd("ZRANGEBYSCORE %s %f %f", key.c_str(), min, max);
    if (!reply || reply->type != REDIS_REPLY_ARRAY)
        return std::move(vec);
    for (size_t i = 0; i < reply->elements; i++)
        vec.push_back(reply->element[i]->str);
    return std::move(vec);
}

int64_t Redis::zrank(const std::string& key, const std::string& member)
{
    ReplyPtr reply = cmd("ZRANK %s %s", key.c_str(), member.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::zrevrank(const std::string& key, const std::string& member)
{
    ReplyPtr reply = cmd("ZREVRANK %s %s", key.c_str(), member.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}

int64_t Redis::zremrangeByScore(const std::string& key, double min, double max)
{
    ReplyPtr reply = cmd("ZREMRANGEBYSCORE %s %f %f", key.c_str(), min, max);
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
        return -1;
    return reply->integer;
}


RedisManager::~RedisManager()
{
    for (auto& i : m_connections)
    {
        for (auto& n : i.second)
            delete n;
    }
}

void RedisManager::add(const std::string& name, const std::string& host, int port,
                       const std::string& passwd, uint32_t connectTimeout,
                       uint32_t cmdTimeout, uint32_t poolSize)
{
    MutexType::Lock lock(m_mutex);
    RedisConf conf;
    conf.host = host;
    conf.port = port;
    conf.passwd = passwd;
    conf.connect_timeout = connectTimeout;
    conf.cmd_timeout = cmdTimeout;
    conf.poolSize = poolSize;
    m_sqlDefines[name] = conf;
}

Redis::ptr RedisManager::get(const std::string& name)
{
    MutexType::Lock lock(m_mutex);
    auto it = m_connections.find(name);
    if (it != m_connections.end())
    {
        if (!it->second.empty())
        {
            Redis* ret = it->second.front();
            it->second.pop_front();
            lock.unlock();
            if (!ret->needToCheck())
            {
                ret->setLastUsedTime(time(0));
                return Redis::ptr(ret, std::bind(&RedisManager::freeRedis, this,
                           name, std::placeholders::_1));
            }
            if (ret->cmd("ping"))
            {
                ret->setLastUsedTime(time(0));
                return Redis::ptr(ret, std::bind(&RedisManager::freeRedis, this,
                           name, std::placeholders::_1));
            }
            else if (ret->reconnect())
            {
                ret->setLastUsedTime(time(0));
                return Redis::ptr(ret, std::bind(&RedisManager::freeRedis, this,
                           name, std::placeholders::_1));
            }
            else
            {
                log_error << "reconnect redis(name: " << name << ") fail";
                return nullptr;
            }
        }
    }
    auto conf = g_redis_config->getValue();
    auto n = conf.find(name);
    if (n == conf.end())
    {
        n = m_sqlDefines.find(name);
        if (n == m_sqlDefines.end())
        {
            log_error << "get redis(name: " << name << ") fail";
            return nullptr;
        }
    }
    lock.unlock();

    Redis* ret = new Redis(n->second.host, n->second.port, n->second.passwd,
                         n->second.connect_timeout, n->second.cmd_timeout,
                         n->second.poolSize);
    if (ret->connect())
    {
        ret->setLastUsedTime(time(0));
        return Redis::ptr(ret, std::bind(&RedisManager::freeRedis, this,
                   name, std::placeholders::_1));
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

void RedisManager::checkConnection(int sec)
{
    time_t now = time(0);
    std::vector<Redis*> connections;
    MutexType::Lock lock(m_mutex);
    for (auto& i : m_connections)
    {
        for (auto it = i.second.begin(); it != i.second.end();)
        {
            if ((int)(now - (*it)->getLastUsedTime()) >= sec)
            {
                connections.push_back(*it);
                i.second.erase(it++);
            }
            else
                it++;
        }
    }
    lock.unlock();
    for (auto& i : connections)
        delete i;
}

// private
void RedisManager::freeRedis(const std::string& name, Redis* r)
{
    MutexType::Lock lock(m_mutex);
    if (m_connections[name].size() < (size_t)r->getPoolSize())
    {
        m_connections[name].push_back(r);
        return;
    }
    delete r;
}

}

}

#endif /*REDIS_ENABLE*/
