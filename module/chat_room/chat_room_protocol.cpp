#include "chat_room_protocol.h"
#include "util.h"

namespace chat_room
{

std::string ChatMessage::get(const std::string& name)
{
    auto it = m_datas.find(name);
    return it == m_datas.end() ? "" : it->second;
}

void ChatMessage::set(const std::string& name, const std::string& val)
{
    m_datas[name] = val;
}

std::string ChatMessage::toString() const
{
    Json::Value json;
    for (auto& i : m_datas)
        json[i.first] = i.second;
    return bifang::JsonUtil::toString(json);
}

// static
ChatMessage::ptr ChatMessage::create(const std::string& v)
{
    Json::Value json = bifang::JsonUtil::fromString(v);

    ChatMessage::ptr msg(new ChatMessage);
    auto names = json.getMemberNames();
    for (auto& i : names)
        msg->m_datas[i] = json[i].asString();

    return msg;
}

}
