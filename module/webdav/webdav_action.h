#ifndef __BIFANG_WEBDAV_ACTION_H
#define __BIFANG_WEBDAV_ACTION_H

#include "bifang.h"
#include "webdav_config.h"
#include "webdav_lock.h"


namespace webdav
{

/**
 * brief: webdav��
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
     * brief: ����http����ȥִ����Ӧ�Ķ���
     */
    void action();

private:
    /**
     * brief: webdav GET ����, �ͻ��������ȡ����˶�Ӧ����Դ
     */
    void get_action();
    /**
     * brief: webdav PUT ����, �ͻ���������Դ���͸������
     */
    void put_action();
    /**
     * brief: webdav DELETE ����, �ͻ�������ɾ������˶�Ӧ����Դ
     */
    void delete_action();
    /**
     * brief: webdav MKCOL ����, �ͻ����������˴����ļ���
     */
    void mkcol_action();
    /**
     * brief: webdav COPY ����, �ͻ����������˸�����Դ��ָ��λ��
     */
    void copy_action();
    /**
     * brief: webdav MOVE ����, �ͻ������������ƶ���Դλ��
     */
    void move_action();
    /**
     * brief: webdav OPTIONS ����, �ͻ��������ȡ�����֧�ֵ���������
     */
    void options_action();
    /**
     * brief: webdav PROPFIND ����, �ͻ��������ȡ��Դ��Ϣ(δ��ȫʵ��, Ӧ�����ݿ�
              ���˴��͹�����xmlѡ�������Ͷ�Ӧ���ֶ�, �����˷���Դ, Ŀǰ���͵���
              Դ�ǹ̶���)
     */
    void propfind_action();
    /**
     * brief: webdav PROPPATCH ����, �ͻ��������޸���Դ��Ϣ(δʵ��, ֻ�ǵ����ؽ�
              ��Դ��Ϣ�ش����˿ͻ���, û�и��ݿͻ��˴��͹�������Ϣȥ�޸���Դ����)
     */
    void proppatch_action();
    /**
     * brief: webdav LOCK ����, �ͻ�������������Դ
     */
    void lock_action();
    /**
     * brief: webdav UNLOCK ����, �ͻ������������Դ
     */
    void unlock_action();
    /**
     * brief: �Բ�֧�ֵ��������ͳһ�Ĵ�����
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
    // �����ļ�·��
    std::string m_path;
    // uri·��
    std::string m_uri_path;
    // �ļ�����
    struct stat* m_stat;
    // http����
    bifang::http::HttpRequest::ptr m_request;
    // http��Ӧ
    bifang::http::HttpResponse::ptr m_response;
};

}

#endif /*__BIFANG_WEBDAV_ACTION_H*/
