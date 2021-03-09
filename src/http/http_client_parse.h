#ifndef __BIFANG_HTTP_CLIENT_PARSE_H
#define __BIFANG_HTTP_CLIENT_PARSE_H


#include "http_common.h"

namespace bifang
{

namespace http
{

struct http_client_parser
{
    int cs;
    size_t content_start;
    int content_len;
    int status;
    int chunked;
    int chunks_done;
    int close;
    size_t nread;
    size_t mark;
    size_t field_start;
    size_t field_len;

    void* data;

    field_cb http_field;
    element_cb reason_phrase;
    element_cb status_code;
    element_cb chunk_size;
    element_cb http_version;
    element_cb header_done;
    element_cb last_chunk;
};

/**
 * brief: 初始化解析器
 * param: parser 解析器指针
 */
void http_client_parser_init(http_client_parser* parser);
/**
 * brief: 执行httpclient解析
 * param: parser 解析器指针
 *        data 待解析数据
 *        len 解析数据长度
 *        off 解析偏移量
 * return: 解析成功的字节数
 */
int http_client_parser_execute(http_client_parser* parser, const char* data, size_t len, size_t off);
/**
 * brief: 判断httpclient解析是否完成
 * param: parser 解析器指针
 * return: -1 - 解析出现错误
 *         1  - 解析已完成
 *         0  - 解析未完成未出现无错误
 */
int http_client_parser_finish(http_client_parser* parser);
/**
 * brief: 判断httpclient解析是否有错误
 * param: parser 解析器指针
 * return: 0    - 成功
 *         其他 - 失败
 */
int http_client_parser_has_error(http_client_parser* parser);
/**
 * brief: 判断httpclient解析是否完成
 * param: parser 解析器指针
 * return: 0    - 失败
 *         其他 - 成功
 */
int http_client_parser_is_finished(http_client_parser* parser);

}

}

#endif /*__BIFANG_HTTP_CLIENT_PARSE_H*/
