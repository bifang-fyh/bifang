/**
 * brief: 简单聊天室模块
 */
#ifndef __BIFANG_CHAT_ROOM_MODULE_H
#define __BIFANG_CHAT_ROOM_MODULE_H


#include "module.h"
#include "chat_room_config.h"

namespace chat_room
{

class ChatRoomModule : public bifang::Module
{
public:
    typedef std::shared_ptr<ChatRoomModule> ptr;

    ChatRoomModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_CHAT_ROOM_MODULE_H*/
