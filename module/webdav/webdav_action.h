#ifndef __BIFANG_WEBDAV_ACTION_H
#define __BIFANG_WEBDAV_ACTION_H

#include "bifang.h"
#include "webdav_config.h"
#include "webdav_lock.h"


namespace webdav
{

/**
 * brief: webdav类
 */
class WebDavAction
{
public:
    typedef std::shared_ptr<WebDavAction> ptr;

    WebDavAction(const std::string& path,
        bifang::http::HttpRequest::ptr request,
        bifang::http::HttpResponse::ptr response);

    ~WebDavAction();

    /**
     * brief: 根据http请求去执行响应的动作
     */
    void action();

private:
    /**
     * brief: webdav GET 请求, 客户端请求获取服务端对应的资源
     */
    void get_action();
    /**
     * brief: webdav PUT 请求, 客户端请求将资源推送给服务端
     */
    void put_action();
    /**
     * brief: webdav DELETE 请求, 客户端请求删除服务端对应的资源
     */
    void delete_action();
    /**
     * brief: webdav MKCOL 请求, 客户端请求服务端创建文件夹
     */
    void mkcol_action();
    /**
     * brief: webdav COPY 请求, 客户端请求服务端复制资源到指定位置
     */
    void copy_action();
    /**
     * brief: webdav MOVE 请求, 客户端请求服务端移动资源位置
     */
    void move_action();
    /**
     * brief: webdav OPTIONS 请求, 客户端请求获取服务端支持的请求类型
     */
    void options_action();
    /**
     * brief: webdav PROPFIND 请求, 客户端请求获取资源信息(未完全实现, 应当根据客
              户端传送过来的xml选项来传送对应的字段, 避免浪费资源, 目前传送的资
              源是固定的)
     */
    void propfind_action();
    /**
     * brief: webdav PROPPATCH 请求, 客户端请求修改资源信息(未实现, 只是单纯地将
              资源信息回传给了客户端, 没有根据客户端传送过来的信息去修改资源属性)
     */
    void proppatch_action();
    /**
     * brief: webdav LOCK 请求, 客户端请求锁定资源
     */
    void lock_action();
    /**
     * brief: webdav UNLOCK 请求, 客户端请求解锁资源
     */
    void unlock_action();
    /**
     * brief: 对不支持的请求进行统一的错误处理
     */
    void error_action();

private:
    bifang::http::HttpStatus dav_verify_lock(const std::string& path, bool delete_lock);

    void dav_error(const std::string& message, bifang::http::HttpStatus not_found);

    int dav_depth(int default_depth);

    time_t dav_timeout();

    uint32_t dav_if();

    uint32_t dav_lock_token();

    std::string dav_propfind(std::vector<bifang::FileUtil::FileInfo>& info,
                    const std::string& uri_path);

    void dav_lock_rsp(bifang::http::HttpStatus status, WebDavLock::ptr webdav_lock);

    std::string dav_format_lockdiscovery(WebDavLock::ptr webdav_lock);

    std::string dav_format_token(uint32_t token, bool brackets);

private:
    // 本地文件路径
    std::string m_path;
    // uri路径
    std::string m_uri_path;
    // 文件属性
    struct stat* m_stat;
    // http请求
    bifang::http::HttpRequest::ptr m_request;
    // http响应
    bifang::http::HttpResponse::ptr m_response;
};

}

#endif /*__BIFANG_WEBDAV_ACTION_H*/
