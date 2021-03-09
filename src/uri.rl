#include "uri.h"

namespace bifang
{

%%{
    # See RFC 3986: http://www.ietf.org/rfc/rfc3986.txt

    machine uri_parser;

    gen_delims = ":" | "/" | "?" | "#" | "[" | "]" | "@";
    sub_delims = "!" | "$" | "&" | "'" | "(" | ")" | "*" | "+" | "," | ";" | "=";
    reserved = gen_delims | sub_delims;
    unreserved = alpha | digit | "-" | "." | "_" | "~";
    pct_encoded = "%" xdigit xdigit;

    action marku { mark = fpc; }

    action save_scheme
    {
        uri->setScheme(std::string(mark, fpc - mark));
        mark = NULL;
    }
    action save_userinfo
    {
        uri->setUserinfo(std::string(mark, fpc - mark));
        mark = NULL;
    }
    action save_host
    {
        uri->setHost(std::string(mark, fpc - mark));
        mark = NULL;
    }
    action save_path
    {
        uri->setPath(std::string(mark, fpc - mark));
        mark = NULL;
    }
    action save_port
    {
        if (fpc != mark)
        {
            uri->setPort(atoi(mark));
        }
        mark = NULL;
    }
    action save_query
    {
        uri->setQuery(std::string(mark, fpc - mark));
        mark = NULL;
    }
    action save_fragment
    {
        uri->setFragment(std::string(mark, fpc - mark));
        mark = NULL;
    }

    scheme = (alpha (alpha | digit | "+" | "-" | ".")*) >marku %save_scheme;
    userinfo = (unreserved | pct_encoded | sub_delims | ":")*;
    dec_octet = digit | [1-9] digit | "1" digit{2} | 2 [0-4] digit | "25" [0-5];
    IPv4address = dec_octet "." dec_octet "." dec_octet "." dec_octet;
    h16 = xdigit{1,4};
    ls32 = (h16 ":" h16) | IPv4address;
    IPv6address = (                         (h16 ":"){6} ls32) |
                  (                    "::" (h16 ":"){5} ls32) |
                  ((             h16)? "::" (h16 ":"){4} ls32) |
                  (((h16 ":"){1} h16)? "::" (h16 ":"){3} ls32) |
                  (((h16 ":"){2} h16)? "::" (h16 ":"){2} ls32) |
                  (((h16 ":"){3} h16)? "::" (h16 ":"){1} ls32) |
                  (((h16 ":"){4} h16)? "::"              ls32) |
                  (((h16 ":"){5} h16)? "::"              h16 ) |
                  (((h16 ":"){6} h16)? "::"                  );
    IPvFuture = "v" xdigit+ "." (unreserved | sub_delims | ":")+;
    IP_literal = "[" (IPv6address | IPvFuture) "]";
    reg_name = (unreserved | pct_encoded | sub_delims)*;
    host = IP_literal | IPv4address | reg_name;
    port = digit*;

# pchar = unreserved | pct_encoded | sub_delims | ":" | "@";
# add (any -- ascii) support chinese
    pchar = ( (any -- ascii ) | unreserved | pct_encoded | sub_delims | ":" | "@" );
    segment = pchar*;
    segment_nz = pchar+;
    segment_nz_nc = (pchar - ":")+;

# path = path-abempty    ; begins with "/" or is empty
#      / path-absolute   ; begins with "/" but not "//"
#      / path-noscheme   ; begins with a non-colon segment
#      / path-rootless   ; begins with a segment
#      / path-empty      ; zero characters
    path_abempty = ("/" segment)* >marku %save_path;
    path_absolute = ("/" (segment_nz ("/" segment)*)?) >marku %save_path;
    path_noscheme = segment_nz_nc ("/" segment)* >marku %save_path;
    path_rootless = segment_nz ("/" segment)* >marku %save_path;
    path_empty = "" >marku %save_path;


    authority = (userinfo >marku %save_userinfo "@")? host >marku %save_host (":" port >marku %save_port)?;

    query = (pchar | "/" | "?")* >marku %save_query;
    fragment = (pchar | "/" | "?")* >marku %save_fragment;

    hier_part = ("//" authority path_abempty) | path_absolute | path_rootless | path_empty;

    relative_part = ("//" authority path_abempty) | path_absolute | path_noscheme | path_empty;

    relative_ref = relative_part ("?" query)? ("#" fragment)?;

    URI = scheme ":" hier_part ("?" query)? ("#" fragment)?;

    URI_reference = URI | relative_ref;

    main := URI_reference;
    write data;
}%%


// static
Uri::ptr Uri::create(const std::string& uristr)
{
    Uri::ptr uri(new Uri);
    std::string t_uristr;
    t_uristr.resize(uristr.size());
    std::transform(uristr.begin(), uristr.end(), t_uristr.begin(), ::tolower);
    int cs = 0;
    const char* mark = 0;
    %% write init;
    const char *p = t_uristr.c_str();
    const char *pe = p + t_uristr.size();
    const char* eof = pe;
    %% write exec;

    if (cs >= uri_parser_first_final)
    {
        return uri;
    }

    return nullptr;
}

Address::ptr Uri::createAddress() const
{
    std::string host = m_host + ":" + std::to_string(getPort());
    auto addr = Address::getAddrInfo(m_host);
    if (addr)
        addr->setPort(getPort());

    return addr;
}

bool Uri::isDefaultPort() const
{
    if (m_port == 0)
        return true;
    else if (m_scheme == "http" || m_scheme == "ws")
        return m_port == 80;
    else if (m_scheme == "https" || m_scheme == "wss")
        return m_port == 443;
    return false;
}

std::string Uri::toString() const
{
    std::stringstream ss;
    ss << m_scheme
       << (m_scheme.empty() ? "" : ":")
       << (m_host.empty() ? "" : "//")
       << m_userinfo
       << (m_userinfo.empty() ? "" : "@")
       << m_host
       << (isDefaultPort() ? "" : ":" + std::to_string(m_port))
       << getPath()
       << (m_query.empty() ? "" : "?")
       << m_query
       << (m_fragment.empty() ? "" : "#")
       << m_fragment;

    return ss.str();
}

}
