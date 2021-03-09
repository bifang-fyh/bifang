#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "http_client_parse.h"
#include "Assert.h"
#include "log.h"

namespace bifang
{

namespace http
{

SystemLogger();

#define LEN(AT, FPC) (FPC - buffer - parser->AT)
#define MARK(M,FPC) (parser->M = (FPC) - buffer)
#define PTR_TO(F) (buffer + parser->F)
#define check(A, M) if (!(A)) { log_error << M; errno=0; goto error; }


/** machine **/
%%{
    machine http_client_parser;

    action mark { MARK(mark, fpc); }

    action start_field { MARK(field_start, fpc); }
    action write_field { parser->field_len = LEN(field_start, fpc); }

    action start_value { MARK(mark, fpc); }
    action write_value
    { 
        if (parser->http_field != NULL)
            parser->http_field(parser->data, PTR_TO(field_start), parser->field_len, PTR_TO(mark), LEN(mark, fpc));
    }

    action write_content_len { parser->content_len = strtol(PTR_TO(mark), NULL, 10); }

    action write_connection_close { parser->close = 1; }

    action reason_phrase
    { 
        if (parser->reason_phrase != NULL)
            parser->reason_phrase(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action status_code
    {
        parser->status = strtol(PTR_TO(mark), NULL, 10);

        if (parser->status_code != NULL)
            parser->status_code(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action http_version
    {
        if (parser->http_version != NULL)
            parser->http_version(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action chunk_size
    {
        parser->chunked = 1;
        parser->content_len = strtol(PTR_TO(mark), NULL, 16);
        parser->chunks_done = parser->content_len <= 0;

        if (parser->chunks_done && parser->last_chunk)
        {
            parser->last_chunk(parser->data, PTR_TO(mark), LEN(mark, fpc));
        }
        else if (parser->chunk_size != NULL)
        {
            parser->chunk_size(parser->data, PTR_TO(mark), LEN(mark, fpc));
        } // else skip it
    }

    action trans_chunked { parser->chunked = 1; }

    action done
    { 
        parser->content_start = fpc - buffer + 1; 
        if (parser->header_done != NULL)
            parser->header_done(parser->data, fpc + 1, pe - fpc - 1);
        fbreak;
    }

# line endings
    CRLF = ("\r\n" | "\n");

# character types
    CTL = (cntrl | 127);
    tspecials = ("(" | ")" | "<" | ">" | "@" | "," | ";" | ":" | "\\" | "\"" | "/" | "[" | "]" | "?" | "=" | "{" | "}" | " " | "\t");

# elements
    token = (ascii -- (CTL | tspecials));

    Reason_Phrase = (any -- CRLF)+ >mark %reason_phrase;
    Status_Code = digit+ >mark %status_code;
    http_number = (digit+ "." digit+);
    HTTP_Version = ("HTTP/" http_number) >mark %http_version;
    Status_Line = HTTP_Version " " Status_Code " " Reason_Phrase :> CRLF;

    field_name = token+ >start_field %write_field;
    field_value = any* >start_value %write_value;
    fields = field_name ":" space* field_value :> CRLF;

    content_length = (/Content-Length/i >start_field %write_field ":" space *
            digit+ >start_value %write_content_len %write_value) CRLF;

    conn_close = (/Connection/i ":" space* /close/i) CRLF %write_connection_close;

    transfer_encoding_chunked = (/Transfer-Encoding/i >start_field %write_field
            ":" space* /chunked/i >start_value %write_value) CRLF @trans_chunked;

    message_header = transfer_encoding_chunked | conn_close | content_length | fields;

    Response = Status_Line (message_header)* CRLF;

    chunk_ext_val = token+;
    chunk_ext_name = token+;
    chunk_extension = (";" chunk_ext_name >start_field %write_field %start_value ("=" chunk_ext_val >start_value)? %write_value )*;
    chunk_size = xdigit+;
    Chunked_Header = chunk_size >mark %chunk_size chunk_extension :> CRLF;

    main := (Response | Chunked_Header) @done;
}%%

/** Data **/
%% write data;

void http_client_parser_init(http_client_parser *parser)
{
    int cs = 0;

    %% write init;

    parser->cs = cs;
    parser->content_start = 0;
    parser->content_len = -1;
    parser->chunked = 0;
    parser->chunks_done = 0;
    parser->mark = 0;
    parser->nread = 0;
    parser->field_len = 0;
    parser->field_start = 0;
    parser->close = 0;
}

int http_client_parser_execute(http_client_parser *parser, const char *buffer, size_t len, size_t off)
{
    parser->nread = 0;

    const char *p, *pe;
    int cs = parser->cs;

    ASSERT_MSG(off <= len, "offset past end of buffer");

    p = buffer + off;
    pe = buffer + len;

    ASSERT_MSG(pe - p == (int)len - (int)off, "pointers aren't same distance");

    %% write exec;

    parser->cs = cs;
    parser->nread += p - (buffer + off);

    ASSERT_MSG(p <= pe, "buffer overflow after parsing execute");
    ASSERT_MSG(parser->nread <= len, "nread longer than length");
    ASSERT_MSG(parser->content_start <= len, "content start after buffer end");
    check(parser->mark < len, "mark is after buffer end");
    check(parser->field_len <= len, "field has length longer than whole buffer");
    check(parser->field_start < len, "field starts after buffer end");

    return parser->nread;

error:
    return -1;
}

int http_client_parser_finish(http_client_parser* parser)
{
    if (http_client_parser_has_error(parser))
        return -1;
    else if (http_client_parser_is_finished(parser))
        return 1;
    else
        return 0;
}

int http_client_parser_has_error(http_client_parser* parser)
{
    return parser->cs == http_client_parser_error;
}

int http_client_parser_is_finished(http_client_parser* parser)
{
    return parser->cs == http_client_parser_first_final;
}

}

}
