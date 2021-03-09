/**
 * brief: html测试文件(默认展示nginx的欢迎界面)
 */
#ifndef __BIFANG_INDEX_HTML_MODULE_H
#define __BIFANG_INDEX_HTML_MODULE_H

#include "module.h"
#include "index_html_config.h"


namespace index_html
{

class IndexHtmlModule: public bifang::Module
{
public:
    typedef std::shared_ptr<IndexHtmlModule> ptr;

    IndexHtmlModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_INDEX_HTML_MODULE_H*/
