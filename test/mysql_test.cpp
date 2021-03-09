/**
 * brief: mysql简单测试
 */
#include "bifang.h"

RootLogger();

/**
 * brief: 通用绑定参数测试
 */
void test_multibind()
{
    const int param_nums = 14;
    bifang::mysql::MySQLStmt stmt(nullptr, nullptr);
    stmt.getBinds().resize(param_nums);
    memset(&stmt.getBinds()[0], 0, sizeof(stmt.getBinds()) * param_nums);
    log_info << "bind param size=" << stmt.getBinds().size();

    int8_t a = 10;
    uint8_t b = 11;
    int16_t c = 12;
    uint16_t d = 13;
    int32_t e = 14;
    uint32_t f = 15;
    int64_t g = 16;
    uint64_t h = 17;
    float i = 18;
    double j = 19;
    std::string k = "214324";
    std::string l = "afdsfdhfhhfhrhtgfdjhgjjhgdj";
    stmt.multibind(a, b, c, d, e, f, g, h, i, j, k, (const void*)&l[0], l.size(), bifang::mysql::MySQLNull(), bifang::mysql::MySQLTime(time(0)));

    std::cout << stmt.toString();
}

void run()
{
#define XX(name, cmd) \
    { \
        bifang::mysql::MySQLRes::ptr res = bifang::MySQLUtil::query(#name, #cmd); \
        if (res) \
        { \
            \
        } \
    }

    XX(test, select * from student);
    XX(sql1, select * from student);
    XX(sql2, select * from student);
    XX(sql3, select * from student);

#undef XX
}

int main(int argc, char* argv[])
{
    bifang::EnvMgr::GetInstance()->init(argc, argv);
    bifang::ConfigMgr::GetInstance()->load("./configs");
    //test_multibind();
    run();

    return 0;
}
