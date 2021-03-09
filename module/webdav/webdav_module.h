#ifndef __BIFANG_WEBDAV_MODULE_H
#define __BIFANG_WEBDAV_MODULE_H

#include "module.h"
#include "webdav_config.h"


namespace webdav
{

class WebDavModule : public bifang::Module
{
public:
    typedef std::shared_ptr<WebDavModule> ptr;

    WebDavModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_WEBDAV_MODULE_H*/
