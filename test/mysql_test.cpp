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

/**
 * brief: 基础增删改查功能测试(有数据会超长插失败, 属于正常现象)
 * details: 自己新建一张表
 *          表名: stduent
 *          列:   code    char(10)
 *                name    varchar(60)
 *                age     int
 *                sex     char(1)
 *                time    timestamp - 默认值设置为CURRENT_TIMESTAMP, 可为null, 设置为自动更新, 不用每次自己去插时间
 */
void test_base()
{
    std::cout << "----------增删改查功能测试----------\n";
#define XX(name, cmd) \
    { \
        bifang::mysql::MySQLRes::ptr res = bifang::MySQLUtil::query(name, cmd); \
        if (res) \
        { \
            std::cout << "总行数: " << res->getRows() << std::endl; \
            std::vector<std::string> field_list; \
            res->getFieldList(field_list); \
            for (auto field : field_list) \
                std::cout << field << "\t"; \
            std::cout << std::endl; \
            \
            while (res->next()) \
            { \
                std::vector<std::string> res_list; \
                res->getResList(res_list); \
                for (auto r : res_list) \
                    std::cout << r << "\t"; \
                std::cout << std::endl; \
            }\
        } \
    }

    const char* code[10] = {
        "1234567890",
        "6564564563",
        "7675675655",
        "6546644564",
        "9808989742",
        "4213213123",
        "1111234454",
        "6465645775",
        "43248778064",
        "7657657657",
    };
    const char* name[10] = {
        "古德",
        "bifang",
        "南希",
        "what ...",
        "gfdfhgfdhfghhfgh",
        "jack",
        "edge",
        "john cena",
        "namename",
        "梵蒂冈的风格的非官方股份个个梵蒂冈梵蒂冈地方官发的话",
    };
    int age[10] = {
        13,
        13,
        11,
        17,
        14,
        14,
        13,
        13,
        12,
        16,
    };
    const char* sex[10] = {
        "Y",
        "Y",
        "N",
        "Y",
        "N",
        "Y",
        "Y",
        "Y",
        "N",
        "Y",
    };

    // 测试插入数据
    for (int i = 0; i < 10; i++)
        bifang::MySQLUtil::execute("sql1", "insert into student (code, name, age, sex) values ('%s', '%s', %d, '%s')", code[i], name[i], age[i], sex[i]);
    std::cout << std::endl;

    std::cout << "更新前查询" << std::endl;
    XX("sql1", "select * from student");
    std::cout << std::endl;

    // 更新刚刚插入的数据
    bifang::MySQLUtil::execute("sql1", "update student set name = '%s', age = %d where code = '%s'", "jackjack", 9, code[5]);
    bifang::MySQLUtil::execute("sql1", "update student set name = '%s', age = %d where code = '%s'", "改名字了", 7, code[9]);
    std::cout << std::endl;

    std::cout << "更新后查询" << std::endl;
    XX("sql1", "select * from student");
    std::cout << std::endl;

    // 删除表内容
#if 0
    for (int i = 0; i < 10; i++)
        bifang::MySQLUtil::execute("sql1", "delete from student where code = '%s'", code[i]);
    std::cout << std::endl;

    std::cout << "删除后查询" << std::endl;
    XX("sql1", "select * from student");
    std::cout << std::endl;
#endif

#undef XX
}

/**
 * brief: 预处理功能测试
 */
void test_stmt()
{
    std::cout << "----------预处理功能测试----------\n";
#define XX(is_convert) \
    { \
        bifang::mysql::MySQLStmtRes::ptr stmt_res = stmt->query(); \
        std::cout << "总行数: " << stmt_res->getRows() << std::endl; \
        std::vector<std::string> field_list; \
        stmt_res->getFieldList(field_list); \
        for (auto field : field_list) \
            std::cout << field << "\t"; \
        std::cout << std::endl; \
        while (stmt_res->next()) \
        { \
            std::vector<std::string> res_list; \
            stmt_res->getResList(res_list, is_convert); \
            for (auto r : res_list) \
                std::cout << r << "\t"; \
            std::cout << std::endl; \
        } \
    }

    bifang::mysql::MySQLStmt::ptr stmt = bifang::MySQLUtil::openPrepare("sql1", "select * from student where sex = ?");

    std::cout << "查询男生信息" << std::endl;
    stmt->multibind("Y");
    XX(true);
    std::cout << std::endl;

    std::cout << "查询女生信息" << std::endl;
    stmt->multibind("N");
    XX(false);
    std::cout << std::endl;

#undef XX
}

/**
 * brief: 事务功能测试
 */
void test_transaction()
{
    std::cout << "----------事务功能测试----------\n";
#define XX(age, auto_commit) \
    { \
        bifang::mysql::MySQLTransaction::ptr transaction = bifang::MySQLUtil::openTransaction("sql1", auto_commit); \
        transaction->begin(); \
        transaction->execute("update student set age = %d where code = '1234567890'", age); \
    }

#define XXX(cmd) \
    { \
        bifang::mysql::MySQLRes::ptr res = bifang::MySQLUtil::query("sql1", cmd); \
        if (res) \
        { \
            std::cout << "总行数: " << res->getRows() << std::endl; \
            std::vector<std::string> field_list; \
            res->getFieldList(field_list); \
            for (auto field : field_list) \
                std::cout << field << "\t"; \
            std::cout << std::endl; \
            \
            while (res->next()) \
            { \
                std::vector<std::string> res_list; \
                res->getResList(res_list); \
                for (auto r : res_list) \
                    std::cout << r << "\t"; \
                std::cout << std::endl; \
            }\
        } \
    }

    std::cout << "执行事务之后提交" << std::endl;
    std::cout << "第一次查询" << std::endl;
    XXX("select * from student");
    XX(100, true);
    std::cout << "第二次查询" << std::endl;
    XXX("select * from student");

    std::cout << std::endl;

    std::cout << "执行事务之后回滚" << std::endl;
    std::cout << "第一次查询" << std::endl;
    XXX("select * from student");
    XX(50, false);
    std::cout << "第二次查询" << std::endl;
    XXX("select * from student");

#undef XX
#undef XXX
}

int main(int argc, char* argv[])
{
    bifang::EnvMgr::GetInstance()->init(argc, argv);
    bifang::ConfigMgr::GetInstance()->load("./configs");
    //test_multibind();
    test_base();
    test_stmt();
    test_transaction();

    return 0;
}
