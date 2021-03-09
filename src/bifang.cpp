#include "bifang.h"

int main(int argc, char* argv[])
{
    if (bifang::ServerMgr::GetInstance()->init(argc, argv))
        return bifang::ServerMgr::GetInstance()->run();

    return 0;
}
