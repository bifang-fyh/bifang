/**
 * brief: 配置文件简单测试
 */
#include "bifang.h"


RootLogger();

// int
bifang::Config<int>::ptr g_value_int =
    bifang::ConfigMgr::GetInstance()->get("system.int", (int)8888, "system int");

// int(must be error)
bifang::Config<float>::ptr g_value_int1 =
    bifang::ConfigMgr::GetInstance()->get("system.int", (float)8888, "system int");

// float
bifang::Config<float>::ptr g_value_float =
    bifang::ConfigMgr::GetInstance()->get("system.float", (float)11.7f, "system float");

// string
bifang::Config<std::string>::ptr g_value_string =
    bifang::ConfigMgr::GetInstance()->get("system.string", std::string("abcdefg"), "system string");

// vector
bifang::Config<std::vector<int> >::ptr g_value_vec =
    bifang::ConfigMgr::GetInstance()->get("system.vector<int>", std::vector<int>{1, 2}, "system vector<int>");

// forward_list
bifang::Config<std::forward_list<int> >::ptr g_value_flist =
    bifang::ConfigMgr::GetInstance()->get("system.forward_list<int>", std::forward_list<int>{1, 2}, "system forward_list<int>");

// list
bifang::Config<std::list<int> >::ptr g_value_list =
    bifang::ConfigMgr::GetInstance()->get("system.list<int>", std::list<int>{1, 2}, "system list<int>");

// set
bifang::Config<std::set<int> >::ptr g_value_set =
    bifang::ConfigMgr::GetInstance()->get("system.set<int>", std::set<int>{1, 2}, "system set<int>");

// unordered_set
bifang::Config<std::unordered_set<int> >::ptr g_value_uset =
    bifang::ConfigMgr::GetInstance()->get("system.unordered_set<int>", std::unordered_set<int>{1, 2}, "system unordered_set<int>");

// map
bifang::Config<std::map<std::string, int> >::ptr g_value_map =
    bifang::ConfigMgr::GetInstance()->get("system.map<std::string, int>", std::map<std::string, int>{{"k", 2}}, "system map<std::string, int>");

// unoredered_map
bifang::Config<std::unordered_map<std::string, int> >::ptr g_value_umap =
    bifang::ConfigMgr::GetInstance()->get("system.unordered_map<std::string, int>", std::unordered_map<std::string, int>{{"k", 2}}, "system unordered_map<std::string, int>");

// 测试配置是否生效
void test_config(const std::string& filename)
{
#define XX(var, message) \
    { \
        auto& v = var->getValue(); \
        for (auto& i : v) \
        { \
            log_info << message ": " << i; \
        } \
        log_info << std::endl << message " json:" << std::endl << var->toString(); \
    }

#define XXX(var, message) \
    { \
        auto& v = var->getValue(); \
        for (auto& i : v) \
        { \
            log_info << message ": {" << i.first << " - " << i.second << "}"; \
        } \
        log_info << std::endl  << message " json:" << std::endl << var->toString(); \
    }

    log_info << "before load int: " << g_value_int->getValue();
    log_info << "before load int json: " << g_value_int->toString();
    log_info << "before load float: " << g_value_float->getValue();
    log_info << "before load float json: " << g_value_float->toString();
    log_info << "before load string: " << g_value_string->getValue();
    log_info << "before load string json: " << g_value_string->toString();
    XX(g_value_vec, "before load vector<int>");
    XX(g_value_flist, "before load forward_list<int>");
    XX(g_value_list, "before load list<int>");
    XX(g_value_set, "before load set<int>");
    XX(g_value_uset, "before load unordered_set<int>");
    XXX(g_value_map, "before load map<std::string, int>");
    XXX(g_value_umap, "before load unordered_map<std::string, int>");

    Json::Value root;
    Json::String errs;
    Json::CharReaderBuilder reader;
    std::ifstream ifs;
    ifs.open(filename);
    if (!Json::parseFromStream(reader, ifs, &root, &errs))
    {
        log_error << "parse json error, strerror=" << errs;
        throw std::logic_error("read file " + filename + " error!");
    }
    bifang::ConfigMgr::GetInstance()->load(root);
    std::cout << std::endl;

    log_info << "after load int: " << g_value_int->getValue();
    log_info << "after load int json: " << g_value_int->toString();
    log_info << "after load float: " << g_value_float->getValue();
    log_info << "after load float json: " << g_value_float->toString();
    log_info << "after load string: " << g_value_string->getValue();
    log_info << "after load string json: " << g_value_string->toString();
    XX(g_value_vec, "after load vector<int>");
    XX(g_value_flist, "after load forward_list<int>");
    XX(g_value_list, "after load list<int>");
    XX(g_value_set, "after load set<int>");
    XX(g_value_uset, "after load unordered_set<int>");
    XXX(g_value_map, "after load map<std::string, int>");
    XXX(g_value_umap, "after load unordered_map<std::string, int>");

#undef XX
#undef XXX
}

// 测试从文件加载参数是否成功
void test_load(const std::string& path)
{
    bifang::ConfigMgr::GetInstance()->load(path);
}

int main(int argc, char* argv[])
{
    bifang::EnvMgr::GetInstance()->init(argc, argv);
    test_config("configs/test.json");
    test_load("./././configs////");

    return 0;
}
