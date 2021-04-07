/**
 * brief: redis简单测试
 */
#include "bifang.h"

RootLogger();

const char* g_keys[10] = {
    "aa",
    "bb",
    "cc",
    "dd",
    "ee",
    "ghfhfghjdjdghjhfkfhjkh",
    "1",
    "gggggggggg",
    "fdgd5645gdf",
    "12112554764",
};
const char* g_values[10] = {
    "1",
    "209",
    "南希",
    "what ...",
    "gfdfhgfdhfghhfgh",
    "jack",
    "edge",
    "john cena",
    "namename",
    "梵蒂冈的风格的非官方股份个个梵蒂冈梵蒂冈地方官发的话",
};
 
#define OUT_START(msg, color, color2) \
    bifang::terminal::set_color(bifang::terminal::color); \
    std::cout << msg << std::endl; \
    bifang::terminal::set_color(bifang::terminal::color2)

#define OUT_END() \
    bifang::terminal::reset_color()

/**
 * brief: 测试redis string类型数据的操作是否正常
 * details: 需先安装并启动redis服务, 并且环境中有hiredis的库
 */
void test_string()
{
    OUT_START("--------------------测试string类型数据操作--------------------", MAGENTA, BLUE);
    OUT_END();
    bifang::redis::Redis::ptr r = bifang::redis::RedisMgr::GetInstance()->get("sql1");

    OUT_START("----------插入数据", BLUE, GREEN);
    for (int i = 0; i < 10; i++)
    {
        std::cout << "插入(key=" << g_keys[i] << ", value=" << g_values[i]
            << (r->set(g_keys[i], g_values[i]) ? ")成功" : ")失败")  << std::endl;
    }
    OUT_END();

    OUT_START("----------查看插入数据的类型以及对应的值", BLUE, GREEN);
    std::unordered_set<std::string> keys = r->keys("*");
    for (auto key : keys)
        std::cout << "key(" << key << ")的类型为" << bifang::redis::TypetoString(r->type(key)) << ", 值为:" << r->get(key) << std::endl;
    OUT_END();

    OUT_START("----------查看插入数据是否存在", BLUE, GREEN);
    std::cout << "key(" << "hahahasf" << ")" << (r->exists("hahahasf") ? "存在" : "不存在") << std::endl;
    std::cout << "key(" << g_keys[7] << ")" << (r->exists(g_keys[7]) ? "存在" : "不存在") << std::endl;
    OUT_END();

    OUT_START("----------测试setnx", BLUE, GREEN);
    std::cout << "设置key(" << "haha11hasf" << (r->setnx("haha11hasf", "0000") ? ")成功" : ")失败") << std::endl;
    std::cout << "设置key(" << g_keys[8] << (r->setnx(g_keys[8], "0000") ? ")成功" : ")失败") << std::endl;
    OUT_END();

    OUT_START("----------数据生存时间", BLUE, GREEN);
    r->expire(g_keys[3], 10000);
    std::cout << "key(" << g_keys[3] << ")的生存时间为" << r->ttl(g_keys[3]) << "秒" << std::endl;
    r->persist(g_keys[3]);
    std::cout << "移除key(" << g_keys[3] << ")的生存时间后, 查询其生存时间后的结果为" << r->ttl(g_keys[3]) << "秒" << std::endl;
    r->setex(g_keys[4], 7777, "kk");
    std::cout << "key(" << g_keys[4] << ")的生存时间为" << r->ttl(g_keys[4]) << "秒" << std::endl;
    r->persist(g_keys[4]);
    std::cout << "移除key(" << g_keys[4] << ")的生存时间后, 查询其生存时间后的结果为" << r->ttl(g_keys[4]) << "秒" << std::endl;
    std::cout << "查询不存在的key(" << "87768578768" << ")的生存时间为" << r->ttl("87768578768") << "秒" << std::endl;
    OUT_END();

    OUT_START("----------测试数据增加减少", BLUE, GREEN);
    std::cout << "key(" << g_keys[0] << ")原始数据为:" << r->get(g_keys[0]) << std::endl;
    r->incr(g_keys[0]);
    r->incrBy(g_keys[0], 100);
    std::cout << "增加101后key(" << g_keys[0] << ")的数据为:" << r->get(g_keys[0]) << std::endl;
    std::cout << "key(" << g_keys[1] << ")原始数据为:" << r->get(g_keys[1]) << std::endl;
    r->decr(g_keys[1]);
    r->decrBy(g_keys[1], 7);
    std::cout << "减少8后key(" << g_keys[1] << ")的数据为:" << r->get(g_keys[1]) << std::endl;
    OUT_END();

    OUT_START("----------删除数据", BLUE, GREEN);
    r->del(r->keys("*"));
    std::cout << "删除全部数据后redis剩余key数量: " << r->keys("*").size() << std::endl;
    OUT_END();
}

void test_list()
{
#define XX(text, v) \
    { \
        std::cout << text ":["; \
        for (size_t i = 0; i < v.size(); i++) \
        { \
            if (i != 0) \
                std::cout << ", "; \
            std::cout << v[i]; \
        } \
        std::cout << "], 共" << v.size() << "个数据" << std::endl; \
    }

    OUT_START("--------------------测试list类型数据操作--------------------", MAGENTA, BLUE);
    OUT_END();
    bifang::redis::Redis::ptr r = bifang::redis::RedisMgr::GetInstance()->get("sql1");
    const std::string key = "array";

    OUT_START("----------插入定位数据到表中", BLUE, GREEN);
    r->lpush(key, {"left", "mid", "right"});
    std::cout << "插入到表中的数据:[left, mid, right], 共3个数据" << std::endl;
    OUT_END();

    OUT_START("----------插入数据到表头", BLUE, GREEN);
    std::cout << "插入到表头的数据:[";
    int count = 0;
    for (int i = 10; i < 20; i++)
    {
        count++;
        if (i != 10)
            std::cout << ", ";
        std::cout << i;
        r->lpush(key, {std::to_string(i)});
    }
    std::cout << "], 共" << count << "个数据" << std::endl;
    OUT_END();

    OUT_START("----------插入数据到表尾", BLUE, GREEN);
    std::cout << "插入到表尾的数据:[";
    count = 0;
    for (int i = 102; i < 118; i++)
    {
        count++;
        if (i != 102)
            std::cout << ", ";
        std::cout << i;
        r->rpush(key, {std::to_string(i)});
    }
    std::cout << "], 共" << count << "个数据" << std::endl;
    OUT_END();

    OUT_START("----------查询列表长度", BLUE, GREEN);
    std::cout << "当前列表长度为:" << r->llen(key) << std::endl;
    OUT_END();

    OUT_START("----------范围查询", BLUE, GREEN);
    std::vector<std::string> vec = r->lrange(key, 0, 1000);
    XX("整个列表", vec);
    vec = r->lrange(key, 0, 10);
    XX("查询范围0到10", vec);
    vec = r->lrange(key, -20, -10);
    XX("查询范围-20到-10", vec);
    OUT_END();

    OUT_START("----------移除列表中的数据", BLUE, GREEN);
    vec = r->lpop(key, 3);
    XX("移除列表头的3个数据", vec);
    vec = r->rpop(key, 5);
    XX("移除列表尾的5个数据", vec);
    OUT_END();

    // 删除数据
    r->del({key});

#undef XX
}

int main(int argc, char* argv[])
{
    bifang::EnvMgr::GetInstance()->init(argc, argv);
    bifang::ConfigMgr::GetInstance()->load("./configs");
    test_string();
    std::cout << std::endl;
    test_list();
    std::cout << std::endl;

    return 0;
}
