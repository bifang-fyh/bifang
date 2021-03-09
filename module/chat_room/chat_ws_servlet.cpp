#include "chat_ws_servlet.h"
#include "chat_room_protocol.h"
#include "bifang.h"

namespace chat_room
{

RootLogger();

namespace chat_session
{

bifang::RWMutex s_mutex;
std::unordered_map<std::string, bifang::ws::WSSession::ptr> s_sessions;

int32_t send(bifang::ws::WSSession::ptr session, ChatMessage::ptr msg)
{
    log_info << msg->toString();
    return session->sendMessage(msg->toString()) > 0 ? 0: 1;
}

bool exists(const std::string& name)
{
    log_info << "session_exists name=" << name;
    bifang::RWMutex::ReadLock lock(s_mutex);
    return s_sessions.find(name) != s_sessions.end();
}

void add(const std::string& name, bifang::ws::WSSession::ptr session)
{
    log_info << "session_add name=" << name;
    bifang::RWMutex::WriteLock lock(s_mutex);
    s_sessions[name] = session;
}

void del(const std::string& name)
{
    log_info << "session_del name=" << name;
    bifang::RWMutex::WriteLock lock(s_mutex);
    s_sessions.erase(name);
}

void notify(ChatMessage::ptr msg,
                bifang::ws::WSSession::ptr session = nullptr)
{
    bifang::RWMutex::ReadLock lock(s_mutex);
    auto sessions = s_sessions;
    lock.unlock();

    for (auto& i : sessions)
    {
        if (i.second == session) // 不发给自己
            continue;
        chat_session::send(i.second, msg);
    }
}

}


ChatWSServlet::ChatWSServlet()
    :bifang::ws::WSServlet("chat_ws_servlet")
{
}

int32_t ChatWSServlet::onConnect(bifang::http::HttpRequest::ptr header,
                           bifang::ws::WSSession::ptr session)
{
    log_info << "ChatWSServlet onConnect";
    return 0;
}

int32_t ChatWSServlet::onClose(bifang::http::HttpRequest::ptr header,
                           bifang::ws::WSSession::ptr session)
{
    auto name = header->getHeader("$name");
    log_info << "ChatWSServlet onClose name=" << name;
    if (!name.empty())
    {
        chat_session::del(name);
        ChatMessage::ptr cmsg(new ChatMessage);
        cmsg->set("type", "user_leave");
        cmsg->set("time", bifang::time_to_string());
        cmsg->set("name", name);
        chat_session::notify(cmsg);
    }
    return 0;
}

int32_t ChatWSServlet::handle(bifang::http::HttpRequest::ptr header,
                           bifang::ws::WSFrameMessage::ptr msg,
                           bifang::ws::WSSession::ptr session)
{
    log_info << "handle=" << session
        << ", opcode=" << msg->getOpcode()
        << ", data=" << msg->getData();

    auto cmsg = ChatMessage::create(msg->getData());
    if (!cmsg)
    {
        log_info << "message is null";
        return -1;
    }

    ChatMessage::ptr rsp(new ChatMessage);
    auto type = cmsg->get("type");
    if (type == "login_request") // 登录
    {
        rsp->set("type", "login_response");
        auto name = cmsg->get("name");
        if (name.empty())
        {
            rsp->set("result", "400");
            rsp->set("msg", "用户名为空");
            return chat_session::send(session, rsp);
        }
        if (chat_session::exists(name))
        {
            rsp->set("result", "402");
            rsp->set("msg", "该用户已登录, 无法重复登录");
            return chat_session::send(session, rsp);
        }
        header->setHeader("$name", name);
        rsp->set("result", "200");
        rsp->set("msg", "登录成功");
        chat_session::add(name, session);

        ChatMessage::ptr nty(new ChatMessage);
        nty->set("type", "user_enter");
        nty->set("time", bifang::time_to_string());
        nty->set("name", name);
        chat_session::notify(nty, session);
        return chat_session::send(session, rsp);
    }
    else if (type == "send_request") // 发送请求
    {
        rsp->set("type", "send_response");
        auto m = cmsg->get("msg");
        if (m.empty())
        {
            rsp->set("result", "500");
            rsp->set("msg", "消息为空");
            return chat_session::send(session, rsp);
        }
        auto name = header->getHeader("$name");
        if (name.empty())
        {
            rsp->set("result", "501");
            rsp->set("msg", "该用户未登录");
            return chat_session::send(session, rsp);
        }
        rsp->set("result", "200");
        rsp->set("msg", "ok");

        ChatMessage::ptr nty(new ChatMessage);
        nty->set("type", "msg");
        nty->set("time", bifang::time_to_string());
        nty->set("name", name);
        nty->set("msg", m);
        chat_session::notify(nty, nullptr);
        return chat_session::send(session, rsp);
    }

    return 0;
}

}
