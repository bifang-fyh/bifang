#ifndef __BIFANG_$2_MODULE_H
#define __BIFANG_$2_MODULE_H

#include "$1_config.h"


namespace $1
{

class $3Module : public bifang::Module
{
public:
    typedef std::shared_ptr<$3Module> ptr;

    $3Module();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_$2_MODULE_H*/
