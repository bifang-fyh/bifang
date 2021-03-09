#ifndef __BIFANG_SERVLET_H
#define __BIFANG_SERVLET_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fnmatch.h>

#include "util.h"
#include "http_session.h"
#include "lock.h"


namespace bifang
{

namespace http
{

/**
 * brief: Servlet类
 */
class Servlet
{
public:
    typedef std::shared_ptr<Servlet> ptr;

    /**
     * brief: 构造函数
     * param: name 名称
     */
    Servlet(const std::string& name)
        :m_name(name)
    {
    }

    /**
     * brief: 析构函数
     */
    virtual ~Servlet() {}

    const std::string& getName() const { return m_name; }

    /**
     * brief: 处理请求
     * param: request HTTP请求
     *        response HTTP响应
     *        session HTTP连接
     * return 是否处理成功
     */
    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) = 0;

protected:
    // 名称
    std::string m_name;
};

/**
 * brief: 函数式Servlet
 */
class FunctionServlet : public Servlet
{
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t (HttpRequest::ptr request,
                              HttpResponse::ptr response,
                              HttpSession::ptr session)> Callback;

    /**
     * brief: 构造函数
     * param: cb 回调函数
     */
    FunctionServlet(Callback cb)
        :Servlet("FunctionServlet")
        ,m_cb(cb)
    {
    }

    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) override
    {
        return m_cb(request, response, session);
    }

private:
    // 回调函数
    Callback m_cb;
};

/**
 * brief: NotFoundServlet(默认返回404)
 */
class NotFoundServlet : public Servlet
{
public:
    typedef std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet();

    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) override;
};

/**
 * brief: Servlet分发器
 */
class ServletDispatch : public Servlet
{
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef RWMutex RWMutexType;

    ServletDispatch();

    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) override;

    /**
     * brief: 添加精准匹配servlet
     * param: uri uri
     *        slt serlvet
     */
    void addServlet(const std::string& uri, Servlet::ptr slt);
    /**
     * brief: 添加精准匹配servlet
     * param: uri uri
     *        cb FunctionServlet回调函数
     */
    void addServlet(const std::string& uri, FunctionServlet::Callback cb);

    /**
     * brief: 添加模糊匹配servlet
     * param[in] uri uri 模糊匹配 /bifang_*
     * param[in] slt servlet
     */
    void addGlobServlet(const std::string& uri, Servlet::ptr slt);
    /**
     * brief: 添加模糊匹配servlet
     * param: uri uri 模糊匹配 /bifang_*
     *        cb FunctionServlet回调函数
     */
    void addGlobServlet(const std::string& uri, FunctionServlet::Callback cb);

    /**
     * brief: 删除精准匹配servlet
     * param: uri uri
     */
    void delServlet(const std::string& uri);
    /**
     * brief: 删除模糊匹配servlet
     * param: uri uri
     */
    void delGlobServlet(const std::string& uri);

    /**
     * brief: 返回默认servlet
     */
    Servlet::ptr getDefault() const { return m_default; }
    /**
     * brief: 设置默认servlet
     * param: v servlet
     */
    void setDefault(Servlet::ptr v) { m_default = v; }

    /**
     * brief: 通过uri获取精准匹配servlet
     * param: uri uri
     * return: 返回对应的servlet
     */
    Servlet::ptr getServlet(const std::string& uri);
    /**
     * brief: 通过uri获取模糊匹配servlet
     * param: uri uri
     * return: 返回对应的servlet
     */
    Servlet::ptr getGlobServlet(const std::string& uri);

    /**
     * brief: 通过uri获取servlet
     * param: uri uri
     * return: 优先精准匹配, 其次模糊匹配, 最后返回默认
     */
    Servlet::ptr getMatchedServlet(const std::string& uri);

    void listServletCreator(std::map<std::string, Servlet::ptr>& infos);
    void listGlobServletCreator(std::map<std::string, Servlet::ptr>& infos);

private:
    // 读写互斥量
    RWMutexType m_mutex;
    // 精准匹配servlet MAP
    // uri(/bifang/xxx) -> servlet
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    // 模糊匹配servlet 数组
    // uri(/bifang/*) -> servlet
    std::vector<std::pair<std::string, Servlet::ptr> > m_globs;
    // 默认servlet，所有路径都没匹配成功时使用
    Servlet::ptr m_default;
};

/**
 * brief: Servlet配置
 */
class ConfigServlet : public Servlet
{
public:
    ConfigServlet();

    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) override;
};

/**
 * brief: Servlet状态
 */
class StatusServlet : public Servlet
{
public:
    StatusServlet();

    virtual int32_t handle(HttpRequest::ptr request,
                        HttpResponse::ptr response,
                        HttpSession::ptr session) override;
};

}

}

#endif /*__BIFANG_SERVLET_H*/
