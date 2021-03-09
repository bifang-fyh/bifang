#include "empty_gif_servlet.h"
#include "bifang.h"


namespace empty_gif
{

RootLogger();

/* the minimal single pixel transparent GIF, 43 bytes */
static const char empty_gif_data[] =
{
    'G', 'I', 'F', '8', '9', 'a',  /* header                                 */

                                   /* logical screen descriptor              */
    0x01, 0x00,                    /* logical screen width                   */
    0x01, 0x00,                    /* logical screen height                  */
    (char)0x80,                    /* global 1-bit color table               */
    0x01,                          /* background color #1                    */
    0x00,                          /* no aspect ratio                        */

                                   /* global color table                     */
    0x00, 0x00, 0x00,              /* #0: black                              */
    (char)0xff, (char)0xff, (char)0xff, /* #1: white                         */

                                   /* graphic control extension              */
    0x21,                          /* extension introducer                   */
    (char)0xf9,                    /* graphic control label                  */
    0x04,                          /* block size                             */
    0x01,                          /* transparent color is given,            */
                                   /*     no disposal specified,             */
                                   /*     user input is not expected         */
    0x00, 0x00,                    /* delay time                             */
    0x01,                          /* transparent color #1                   */
    0x00,                          /* block terminator                       */

                                   /* image descriptor                       */
    0x2c,                          /* image separator                        */
    0x00, 0x00,                    /* image left position                    */
    0x00, 0x00,                    /* image top position                     */
    0x01, 0x00,                    /* image width                            */
    0x01, 0x00,                    /* image height                           */
    0x00,                          /* no local color table, no interlaced    */

                                   /* table based image data                 */
    0x02,                          /* LZW minimum code size,                 */
                                   /*     must be at least 2-bit             */
    0x02,                          /* block size                             */
    0x4c, 0x01,                    /* compressed bytes 01_001_100, 0000000_1 */
                                   /* 100: clear code                        */
                                   /* 001: 1                                 */
                                   /* 101: end of information code           */
    0x00,                          /* block terminator                       */

    0x3B                           /* trailer                                */
};

EmptyGifServlet::EmptyGifServlet()
    :bifang::http::Servlet("EmptyGifServlet")
{
}

// override
int32_t EmptyGifServlet::handle(bifang::http::HttpRequest::ptr request,
                             bifang::http::HttpResponse::ptr response,
                             bifang::http::HttpSession::ptr session)
{

    response->setConetentType("*.gif");
    if (response->setLastModified("", request, "Tue, 19 Jan 2021 15:40:41 GMT"))
        return 0;
    response->setContent(std::string(empty_gif_data, sizeof(empty_gif_data)));

    return 0;
}

}
