/**
 * brief: json简单测试
 */
#include "bifang.h"

void test()
{
    std::vector<std::string> files;
    bifang::FileUtil::ListAllFile(files, "./configs", ".json");

    for (auto& i : files)
    {
        Json::Value root;
        Json::String errs;
        Json::CharReaderBuilder reader;
        std::ifstream ifs;
        ifs.open(i);
        if (!Json::parseFromStream(reader, ifs, &root, &errs))
        {
            std::cout << "parse json strerror: " << errs << std::endl;
            return;
        }

        std::cout << i << std::endl << root << std::endl;
    }
}

int main(int argc, char* argv[])
{
    bifang::EnvMgr::GetInstance()->init(argc, argv);
    test();

    return 0;
}
