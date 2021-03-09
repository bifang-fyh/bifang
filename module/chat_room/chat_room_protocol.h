/**
 * brief: 自定义json协议
 */
#ifndef __BIFANG_CHAT_ROOM_PROTOCOL_H
#define __BIFANG_CHAT_ROOM_PROTOCOL_H

#include <map>
#include <memory>

namespace chat_room
{

class ChatMessage
{
public:
    typedef std::shared_ptr<ChatMessage> ptr;

    std::string get(const std::string& name);
    void set(const std::string& name, const std::string& val);
    std::string toString() const;

public:
    static ChatMessage::ptr create(const std::string& v);

private:
    std::map<std::string, std::string> m_datas;
};

}

#endif /*__BIFANG_CHAT_ROOM_PROTOCOL_H*/
