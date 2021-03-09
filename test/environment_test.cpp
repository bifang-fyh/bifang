/**
 * brief: environment简单测试
 */
#include "bifang.h"


int main(int argc, char* argv[])
{
    std::cout << "argc=" << argc << std::endl;
    bifang::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    bifang::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    bifang::EnvMgr::GetInstance()->addHelp("p", "print help");
    if (!bifang::EnvMgr::GetInstance()->init(argc, argv))
    {
        bifang::EnvMgr::GetInstance()->printHelp();
        return 0;
    }
    bifang::EnvMgr::GetInstance()->printArgs();
    std::cout << "exe=" << bifang::EnvMgr::GetInstance()->getExe() << std::endl;
    std::cout << "cwd=" << bifang::EnvMgr::GetInstance()->getCwd() << std::endl;
    std::cout << "path=" << bifang::EnvMgr::GetInstance()->getEnv("PATH", "xxx") << std::endl;
    std::cout << "test=" << bifang::EnvMgr::GetInstance()->getEnv("TEST", "") << std::endl;
    bifang::EnvMgr::GetInstance()->setEnv("TEST", "yy");
    std::cout << "test=" << bifang::EnvMgr::GetInstance()->getEnv("TEST", "") << std::endl;
    if (bifang::EnvMgr::GetInstance()->has("p"))
        bifang::EnvMgr::GetInstance()->printHelp();

    return 0;
}
