#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Assert.h"
#include "http_server_parse.h"

namespace bifang
{

namespace http
{

SystemLogger();

#define LEN(AT, FPC) (FPC - buffer - parser->AT)
#define MARK(M, FPC) (parser->M = (FPC) - buffer)
#define PTR_TO(F) (buffer + parser->F)

/** Machine **/
%%{
    machine http_server_parser;

    action mark { MARK(mark, fpc); }


    action start_field { MARK(field_start, fpc); }
    action write_field { parser->field_len = LEN(field_start, fpc); }

    action start_value { MARK(mark, fpc); }
    action write_value
    {
        if (parser->http_field != NULL)
            parser->http_field(parser->data, PTR_TO(field_start), parser->field_len, PTR_TO(mark), LEN(mark, fpc));
    }

    action request_method
    { 
        if (parser->request_method != NULL) 
            parser->request_method(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action request_uri
    { 
        if (parser->request_uri != NULL)
            parser->request_uri(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action fragment
    {
        if (parser->fragment != NULL)
            parser->fragment(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action start_query { MARK(query_start, fpc); }
    action query_string
    { 
        if(parser->query_string != NULL)
            parser->query_string(parser->data, PTR_TO(query_start), LEN(query_start, fpc));
    }

    action http_version
    {
        if(parser->http_version != NULL)
            parser->http_version(parser->data, PTR_TO(mark), LEN(mark, fpc));
    }

    action request_path
    {
        if(parser->request_path != NULL)
            parser->request_path(parser->data, PTR_TO(mark), LEN(mark,fpc));
    }

    action done
    {
        if(parser->xml_sent || parser->json_sent)
        {
            parser->content_start = PTR_TO(mark) - buffer;
            // +1 includes the \0
            parser->content_len = fpc - buffer - parser->content_start + 1;
        }
        else
        {
            parser->content_start = fpc - buffer + 1;

            if(parser->header_done != NULL)
                parser->header_done(parser->data, fpc + 1, pe - fpc - 1);
        }
        fbreak;
    }

    action xml { parser->xml_sent = 1; }

    action json { parser->json_sent = 1; }


#### HTTP PROTOCOL GRAMMAR
    CRLF = ( "\r\n" | "\n" );

    # URI description as per RFC 3986.

    more_delims   = ( "{" | "}" | "^" ) when { parser->uri_relaxed };
    sub_delims    = ( "!" | "$" | "&" | "'" | "(" | ")" | "*"
                  | "+" | "," | ";" | "=" | more_delims );
    gen_delims    = ( ":" | "/" | "?" | "#" | "[" | "]" | "@" );
    reserved      = ( gen_delims | sub_delims );
    unreserved    = ( alpha | digit | "-" | "." | "_" | "~" );

    pct_encoded   = ( "%" xdigit xdigit );

# pchar         = ( unreserved | pct_encoded | sub_delims | ":" | "@" ) ;
# add (any -- ascii) support chinese
    pchar         = ( (any -- ascii) | unreserved | pct_encoded | sub_delims | ":" | "@" );

    fragment      = ( ( pchar | "/" | "?" )* ) >mark %fragment;

    query         = ( ( pchar | "/" | "?" )* ) %query_string;

    # non_zero_length segment without any colon ":" ) ;
    segment_nz_nc = ( ( unreserved | pct_encoded | sub_delims | "@" )+ );
    segment_nz    = ( pchar+ );
    segment       = ( pchar* );

    path_empty    = ( "" );
    path_rootless = ( segment_nz ( "/" segment )* );
    path_noscheme = ( segment_nz_nc ( "/" segment )* );
    path_absolute = ( "/" ( segment_nz ( "/" segment )* )? );
    path_abempty  = ( ( "/" segment )* );

    path          = ( path_abempty    # begins with "/" or is empty
                    | path_absolute   # begins with "/" but not "//"
                    | path_noscheme   # begins with a non-colon segment
                    | path_rootless   # begins with a segment
                    | path_empty      # zero characters
                    );

    reg_name      = ( unreserved | pct_encoded | sub_delims )*;

    dec_octet     = ( digit                 # 0-9
                    | ("1"-"9") digit       # 10-99
                    | "1" digit{2}          # 100-199
                    | "2" ("0"-"4") digit   # 200-249
                    | "25" ("0"-"5")        # 250-255
                    );

    IPv4address   = ( dec_octet "." dec_octet "." dec_octet "." dec_octet );
    h16           = ( xdigit{1,4} );
    ls32          = ( ( h16 ":" h16 ) | IPv4address );

    IPv6address   = (                               6( h16 ":" ) ls32
                    |                          "::" 5( h16 ":" ) ls32
                    | (                 h16 )? "::" 4( h16 ":" ) ls32
                    | ( ( h16 ":" ){1,} h16 )? "::" 3( h16 ":" ) ls32
                    | ( ( h16 ":" ){2,} h16 )? "::" 2( h16 ":" ) ls32
                    | ( ( h16 ":" ){3,} h16 )? "::"    h16 ":"   ls32
                    | ( ( h16 ":" ){4,} h16 )? "::"              ls32
                    | ( ( h16 ":" ){5,} h16 )? "::"              h16
                    | ( ( h16 ":" ){6,} h16 )? "::"
                    ) ;

    IPvFuture     = ( "v" xdigit+ "." ( unreserved | sub_delims | ":" )+ );

    IP_literal    = ( "[" ( IPv6address | IPvFuture  ) "]" );

    port          = ( digit* );
    host          = ( IP_literal | IPv4address | reg_name );
    userinfo      = ( ( unreserved | pct_encoded | sub_delims | ":" )* );
    authority     = ( ( userinfo "@" )? host ( ":" port )? );

    scheme        = ( alpha ( alpha | digit | "+" | "-" | "." )* );

    relative_part = ( "//" authority path_abempty
                    | path_absolute
                    | path_noscheme
                    | path_empty
                    );


    hier_part     = ( "//" authority path_abempty
                    | path_absolute
                    | path_rootless
                    | path_empty
                    );

    absolute_URI  = ( scheme ":" hier_part ( "?" query )? );

    relative_ref  = ( (relative_part %request_path ( "?" %start_query query )?) >mark %request_uri ( "#" fragment )? );
    URI           = ( scheme ":" (hier_part  %request_path ( "?" %start_query query )?) >mark %request_uri ( "#" fragment )? );

    URI_reference = ( URI | relative_ref );

# HTTP header parsing
    Method = ( upper | digit ){1,20} >mark %request_method;

    http_number = ( "1." ("0" | "1") );
    HTTP_Version = ( "HTTP/" http_number ) >mark %http_version;
    Request_Line = ( Method " " URI_reference " " HTTP_Version CRLF );

    HTTP_CTL = (0 - 31) | 127;
    HTTP_separator = ( "(" | ")" | "<" | ">" | "@"
                    | "," | ";" | ":" | "\\" | "\""
                    | "/" | "[" | "]" | "?" | "="
                    | "{" | "}" | " " | "\t"
                    );

    lws = CRLF? (" " | "\t")+;
    token = ascii -- ( HTTP_CTL | HTTP_separator );
    content = ((any -- HTTP_CTL) | lws);

    field_name = ( token )+ >start_field %write_field;

    field_value = content* >start_value %write_value;

    message_header = field_name ":" lws* field_value :> CRLF;

    Request = Request_Line ( message_header )* ( CRLF );

    SocketJSONStart = ("@" relative_part);
    SocketJSONData = "{" any* "}" :>> "\0";

    SocketXMLData = ("<" [a-z0-9A-Z\-.]+) >mark %request_path ("/" | space | ">") any* ">" :>> "\0";

    SocketJSON = SocketJSONStart >mark %request_path " " SocketJSONData >mark @json;
    SocketXML = SocketXMLData @xml;

    SocketRequest = (SocketXML | SocketJSON);

    main := (Request | SocketRequest) @done;
}%%

/** Data **/
%% write data;

void http_server_parser_init(http_server_parser* parser)
{
    int cs = 0;

    %% write init;

    parser->cs = cs;
    parser->content_start = 0;
    parser->content_len = 0;
    parser->mark = 0;
    parser->nread = 0;
    parser->field_len = 0;
    parser->field_start = 0;
    parser->xml_sent = 0;
    parser->json_sent = 0;
}

size_t http_server_parser_execute(http_server_parser* parser, const char* buffer, size_t len, size_t off)  
{
    if (len == 0) return 0;
    parser->nread = 0;

    const char *p, *pe;
    int cs = parser->cs;

    ASSERT_MSG(off <= len, "offset past end of buffer");

    p = buffer + off;
    pe = buffer + len;

    ASSERT_MSG(pe - p == (int)len - (int)off, "pointers aren't same distance");

    %% write exec;

    ASSERT_MSG(p <= pe, "Buffer overflow after parsing");

    if (!http_server_parser_has_error(parser))
        parser->cs = cs;

    parser->nread += p - (buffer + off);

    ASSERT_MSG(parser->nread <= len, "nread longer than length");
    ASSERT_MSG(parser->content_start <= len, "content start after buffer end");
    ASSERT_MSG(parser->mark < len, "mark is after buffer end");
    ASSERT_MSG(parser->field_len <= len, "field has length longer than whole buffer");
    ASSERT_MSG(parser->field_start < len, "field starts after buffer end");

    return parser->nread;
}

int http_server_parser_finish(http_server_parser* parser)
{
    if (http_server_parser_has_error(parser))
        return -1;
    else if (http_server_parser_is_finished(parser))
        return 1;
    else
        return 0;
}

int http_server_parser_has_error(http_server_parser* parser)
{
    return parser->cs == http_server_parser_error;
}

int http_server_parser_is_finished(http_server_parser* parser)
{
    return parser->cs >= http_server_parser_first_final;
}

}

}
