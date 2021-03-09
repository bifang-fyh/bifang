/**
 * brief: ≈‰÷√Œƒº˛
 */
#ifndef __BIFANG_WEBDAV_CONFIG_H
#define __BIFANG_WEBDAV_CONFIG_H

#include "version.h"

namespace webdav
{

#define MODULE_NAME         "webdav"
#define MODULE_VERSION      "1.0"
#define URI_PATH            "/webdav"

#define AUTH_REALM          "please input user/passwd"
#define AUTH_PATH           "configs/bifang.passwd"

//#define LIMIT_RATE        (50)
//#define LIMIT_RATE_AFTER  (10 * 1024)

#define DAV_INFINITY_DEPTH  (-1)
#define DAV_INVALID_DEPTH   (-2)
#define LOCK_MAX_TIMEOUT    (2 * 60)


#define ROOT_PATH    "/home/test_smb"


}

#endif /*__BIFANG_WEBDAV_CONFIG_H*/
