/**
 * brief: 字符编码类简单测试
 */
#include "bifang.h"

RootLogger();

static void transfer(const std::string& toname, const std::string& fromname)
{
    std::string from_name = "./configs/" + fromname + ".txt";
    std::string to_name = "./logs/" + fromname + "_to_" + toname + ".txt";

    std::ifstream ifs(from_name);
    if (!ifs.is_open())
    {
        log_error << "open " << from_name << " error";
        return;
    }
    std::ofstream ofs(to_name, std::ios::trunc);
    if (!ofs.is_open())
    {
        log_error << "open " << to_name << " error";
        return;
    }
    std::stringstream in;
    std::string out;
    std::stringstream buffer;
    in << ifs.rdbuf();

    bifang::Iconv::ptr ico(new bifang::Iconv(toname, fromname));
    ico->change(in.str(), out);
    ofs << out;
}

int main(int argc, char* argv[])
{
    log_info << "main begin";

    transfer("UTF-8", "GBK");
    transfer("UTF-8", "GB18030");
    transfer("GBK", "UTF-8");
    transfer("GB18030", "UTF-8");

    log_info << "main end";

    return 0;
}
