#ifndef __BIFANG_EMPTY_GIF_MODULE_H
#define __BIFANG_EMPTY_GIF_MODULE_H

#include "module.h"
#include "empty_gif_config.h"


namespace empty_gif
{

class EmptyGifModule : public bifang::Module
{
public:
    typedef std::shared_ptr<EmptyGifModule> ptr;

    EmptyGifModule();

    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif /*__BIFANG_EMPTY_GIF_MODULE_H*/
