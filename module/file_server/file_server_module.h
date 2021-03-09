#ifndef __BIFANG_FILE_SERVER_MODULE_H
#define __BIFANG_FILE_SERVER_MODULE_H

#include "module.h"
#include "file_server_config.h"


namespace file_server
{

class FileServerModule : public bifang::Module
{
public:
    typedef std::shared_ptr<FileServerModule> ptr;

    FileServerModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_FILE_SERVER_MODULE_H*/
