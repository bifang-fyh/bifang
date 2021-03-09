/**
 * brief: websocket客户端简单测试
 */
#include "bifang.h"

RootLogger();

void run()
{
    auto ret = bifang::ws::WSClient::create("http://127.0.0.1:8888/bifang", 5000);
    if (!ret.second)
    {
        log_error << std::endl << ret.first->toString() << std::endl;
        return;
    }

    auto conn = ret.second;
    while (1)
    {
        for (size_t i = 0; i < 7; i++)
            conn->sendMessage(bifang::StringUtil::randomString(50), bifang::ws::WSFrameHead::TEXT_FRAME, false);
        conn->sendMessage(bifang::StringUtil::randomString(121), bifang::ws::WSFrameHead::TEXT_FRAME, true);
        auto msg = conn->recvMessage();
        if (!msg)
            break;

        sleep(3);
    }
}

int main(int argc, char* argv[])
{
    bifang::IOManager iom(2);
    iom.schedule(run);

    return 0;
}
