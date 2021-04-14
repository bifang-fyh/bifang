#ifndef __BIFANG_DOXYGEN_MODULE_H
#define __BIFANG_DOXYGEN_MODULE_H

#include "doxygen_config.h"


namespace doxygen
{

class DoxygenModule : public bifang::Module
{
public:
    typedef std::shared_ptr<DoxygenModule> ptr;

    DoxygenModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_DOXYGEN_MODULE_H*/
