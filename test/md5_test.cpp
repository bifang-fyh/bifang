/**
 * brief: MD5简单测试
 */
#include "bifang.h"

RootLogger();

void test()
{
    std::vector<std::string> files;
    bifang::FileUtil::ListAllFile(files, "configs", ".json");

    for (size_t i = 0; i < files.size(); i++)
    {
        std::ifstream ifs(files[i]);
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        //log_info << files[i] << ":" << std::endl << buffer.str();
        log_info << files[i] << std::endl << " MD5(16): " << bifang::CryptUtil::MD5Create16(buffer.str(), i % 2 == 0);
        log_info << files[i] << std::endl << " MD5(32): " << bifang::CryptUtil::MD5Create32(buffer.str(), i % 2 == 0);
    }
}

int main(int argc, char* argv[])
{
    test();

    return 0;
}
