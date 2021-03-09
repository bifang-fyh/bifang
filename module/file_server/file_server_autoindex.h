#ifndef __BIFANG_FILE_SERVER_AUTOINDEX_H
#define __BIFANG_FILE_SERVER_AUTOINDEX_H

#include "bifang.h"
#include "file_server_config.h"


namespace file_server
{

class AutoIndex
{
public:
    typedef std::shared_ptr<AutoIndex> ptr;

    AutoIndex(const std::string& path);

    bool init();

    std::string toString(const std::string& label);

private:
    std::string m_path;
    std::vector<bifang::FileUtil::FileInfo> m_info;
};

}

#endif /*__BIFANG_FILE_SERVER_AUTOINDEX_H*/
