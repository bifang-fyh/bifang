/**
 * brief: ≈‰÷√Œƒº˛
 */
#ifndef __BIFANG_FILE_SERVER_CONFIG_H
#define __BIFANG_FILE_SERVER_CONFIG_H

#include "version.h"

namespace file_server
{

#define MODULE_NAME       "file_server"
#define MODULE_VERSION    "1.0"
#define URI_PATH          "/file_server"
#define MAX_NAME_LEN      60

#define AUTH_REALM        "please input user/passwd"
#define AUTH_PATH         "configs/bifang.passwd"

//#define LIMIT_RATE        (88)
//#define LIMIT_RATE_AFTER  (10 * 1024)

#define ROOT_PATH    "/home/bifang/src"

}

#endif /*__BIFANG_FILE_SERVER_CONFIG_H*/
