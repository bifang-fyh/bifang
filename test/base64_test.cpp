/**
 * brief: base64简单测试
 */
#include "bifang.h"

void test()
{
    std::cout << "--- base64 encode begin ---" << std::endl;
    {
        std::ifstream ifs("configs/log.json");
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string des = bifang::CryptUtil::base64_encode(buffer.str());

        std::ofstream ofs("logs/log_encode");
        ofs << des;
    }
    std::cout << "--- base64 encode end ---" << std::endl;

    std::cout << "--- base64 decode begin ---" << std::endl;
    {
        std::ifstream ifs("logs/log_encode");
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string des = bifang::CryptUtil::base64_decode(buffer.str());

        std::ofstream ofs("logs/log_decode");
        ofs << des;
    }
    std::cout << "--- base64 decode end ---" << std::endl;
}


int main(int argc, char* argv[])
{
    test();

    return 0;
}
