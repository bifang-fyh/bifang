/**
 * brief: ÅäÖÃÎÄ¼þ
 */
#ifndef __BIFANG_INDEX_HTML_CONFIG_H
#define __BIFANG_INDEX_HTML_CONFIG_H

namespace index_html
{

#define MODULE_NAME     "index_html"
#define MODULE_VERSION  "1.0"
#define DEFAULT_FILE    "index.html"
#define URI_PATH        "/index_html"
#define ROOT_PATH       bifang::EnvMgr::GetInstance()->getCwd() + "html" + URI_PATH

}

#endif /*__BIFANG_INDEX_HTML_CONFIG_H*/
