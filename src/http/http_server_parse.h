#ifndef __BIFANG_HTTP_SERVER_PARSE_H
#define __BIFANG_HTTP_SERVER_PARSE_H

#include "http_common.h"

namespace bifang
{

namespace http
{

struct http_server_parser
{ 
    int cs;
    size_t content_start;
    int content_len;
    size_t nread;
    size_t mark;
    size_t field_start;
    size_t field_len;
    size_t query_start;
    int xml_sent;
    int json_sent;

    void* data;

    int uri_relaxed;
    field_cb http_field;
    element_cb request_method;
    element_cb request_uri;
    element_cb fragment;
    element_cb request_path;
    element_cb query_string;
    element_cb http_version;
    element_cb header_done;
};

/**
 * brief: 初始化解析器
 * param: parser 解析器指针
 */
void http_server_parser_init(http_server_parser* parser);
/**
 * brief: 执行http解析
 * param: parser 解析器指针
 *        data 待解析数据
 *        len 解析数据长度
 *        off 解析偏移量
 * return: 解析成功的字节数
 */
size_t http_server_parser_execute(http_server_parser* parser, const char *data, size_t len, size_t off);
/**
 * brief: 判断http解析是否完成
 * param: parser 解析器指针
 * return: -1 - 解析出现错误
 *         1  - 解析已完成
 *         0  - 解析未完成未出现无错误
 */
int http_server_parser_finish(http_server_parser* parser);
/**
 * brief: 判断http解析是否有错误
 * param: parser 解析器指针
 * return: 0    - 成功
 *         其他 - 失败
 */
int http_server_parser_has_error(http_server_parser* parser);
/**
 * brief: 判断http解析是否完成
 * param: parser 解析器指针
 * return: 0    - 失败
 *         其他 - 成功
 */
int http_server_parser_is_finished(http_server_parser* parser);

}

}

#endif /*__BIFANG_HTTP_SERVER_PARSE_H*/
