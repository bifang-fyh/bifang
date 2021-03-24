#include "http_common.h"
#include "stream/zlib_stream.h"
#include "log.h"

namespace bifang
{

namespace http
{

SystemLogger();

static const std::unordered_map<std::string, std::string> ContentTypeList = 
{
    {"ai",      "application/postscript"},
    {"a11",     "application/x-a11"},
    {"anv",     "application/x-anv"},
    {"awf",     "application/vnd.adobe.workflow"},
    {"bmp",     "application/x-bmp"},
    {"bot",     "application/x-bot"},
    {"cal",     "application/x-cals"},
    {"cat",     "application/vnd.ms-pki.seccat"},
    {"cdf",     "application/x-netcdf"},
    {"cdr",     "application/x-cdr"},
    {"cel",     "application/x-cel"},
    {"cer",     "application/x-x509-ca-cert"},
    {"cg4",     "application/x-g4"},
    {"cgm",     "application/x-cgm"},
    {"cit",     "application/x-cit"},
    {"cmp",     "application/x-cmp"},
    {"cmx",     "application/x-cmx"},
    {"cot",     "application/x-cot"},
    {"crl",     "application/pkix-crl"},
    {"crt",     "application/x-x509-ca-cert"},
    {"csi",     "application/x-csi"},
    {"cut",     "application/x-cut"},
    {"dbf",     "application/x-dbf"},
    {"dbm",     "application/x-dbm"},
    {"dbx",     "application/x-dbx"},
    {"dcx",     "application/x-dcx"},
    {"der",     "application/x-x509-ca-cert"},
    {"dgn",     "application/x-dgn"},
    {"dib",     "application/x-dib"},
    {"dll",     "application/x-msdownload"},
    {"doc",     "application/msword"},
    {"dot",     "application/msword"},
    {"drw",     "application/x-drw"},
    {"dwf",     "application/x-dwf"},
    {"dwg",     "application/x-dwg"},
    {"dxb",     "application/x-dxb"},
    {"dxf",     "application/x-dxf"},
    {"edn",     "application/vnd.adobe.edn"},
    {"emf",     "application/x-emf"},
    {"epi",     "application/x-epi"},
    {"eps",     "application/postscript"},
    {"etd",     "application/x-ebx"},
    {"exe",     "application/x-msdownload"},
    {"fdf",     "application/vnd.fdf"},
    {"fif",     "application/fractals"},
    {"frm",     "application/x-frm"},
    {"g4",      "application/x-g4"},
    {"gbr",     "application/x-gbr"},
    {"gl2",     "application/x-gl2"},
    {"gp4",     "application/x-gp4"},
    {"hgl",     "application/x-hgl"},
    {"hmr",     "application/x-hmr"},
    {"hpg",     "application/x-hpgl"},
    {"hpl",     "application/x-hpl"},
    {"hqx",     "application/mac-binhex40"},
    {"hrf",     "application/x-hrf"},
    {"hta",     "application/hta"},
    {"icb",     "application/x-icb"},
    {"iff",     "application/x-iff"},
    {"ig4",     "application/x-g4"},
    {"igs",     "application/x-igs"},
    {"iii",     "application/x-iphone"},
    {"img",     "application/x-img"},
    {"ins",     "application/x-internet-signup"},
    {"isp",     "application/x-internet-signup"},
    {"js",      "application/x-javascript"},
    {"ls",      "application/x-javascript"},
    {"ltr",     "application/x-ltr"},
    {"mac",     "application/x-mac"},
    {"man",     "application/x-troff-man"},
    {"lbm",     "application/x-lbm"},
    {"mdb",     "application/msaccess"},
    {"mfp",     "application/x-shockwave-flash"},
    {"latex",   "application/x-latex"},
    {"lar",     "application/x-laplayer-reg"},
    {"mi",      "application/x-mi"},
    {"out",     "application/x-out"},
    {"pcl",     "application/x-pcl"},
    {"pcx",     "application/x-pcx"},
    {"pdf",     "application/pdf"},
    {"pdx",     "application/vnd.adobe.pdx"},
    {"pfx",     "application/x-pkcs12"},
    {"pgl",     "application/x-pgl"},
    {"pic",     "application/x-pic"},
    {"pko",     "application/vnd.ms-pki.pko"},
    {"pl",      "application/x-perl"},
    {"pot",     "application/vnd.ms-powerpoint"},
    {"ppa",     "application/vnd.ms-powerpoint"},
    {"ppm",     "application/x-ppm"},
    {"pps",     "application/vnd.ms-powerpoint"},
    {"ppt",     "application/vnd.ms-powerpoint"},
    {"pr",      "application/x-pr"},
    {"prf",     "application/pics-rules"},
    {"prn",     "application/x-prn"},
    {"prt",     "application/x-prt"},
    {"ps",      "application/postscript"},
    {"ptn",     "application/x-ptn"},
    {"pwz",     "application/vnd.ms-powerpoint"},
    {"mpd",     "application/vnd.ms-project"},
    {"mocha",   "application/x-javascript"},
    {"mil",     "application/x-mil"},
    {"mpp",     "application/vnd.ms-project"},
    {"mpt",     "application/vnd.ms-project"},
    {"mpw",     "application/vnd.ms-project"},
    {"mpx",     "application/vnd.ms-project"},
    {"rec",     "application/vnd.rn-recording"},
    {"red",     "application/x-red"},
    {"rgb",     "application/x-rgb"},
    {"rjs",     "application/vnd.rn-realsystem-rjs"},
    {"rjt",     "application/vnd.rn-realsystem-rjt"},
    {"rlc",     "application/x-rlc"},
    {"rle",     "application/x-rle"},
    {"wmz",     "application/x-ms-wmz"},
    {"wp6",     "application/x-wp6"},
    {"wpd",     "application/x-wpd"},
    {"wpg",     "application/x-wpg"},
    {"wpl",     "application/vnd.ms-wpl"},
    {"wq1",     "application/x-wq1"},
    {"wr1",     "application/x-wr1"},
    {"wri",     "application/x-wri"},
    {"wrk",     "application/x-wrk"},
    {"ws",      "application/x-ws"},
    {"ws2",     "application/x-ws"},
    {"xdp",     "application/vnd.adobe.xdp"},
    {"xfd",     "application/vnd.adobe.xfd"},
    {"xfdf",    "application/vnd.adobe.xfdf"},
    {"xls",     "application/vnd.ms-excel"},
    {"xlsx",    "application/vnd.ms-excel"},
    {"xlw",     "application/x-xlw"},
    {"xwd",     "application/x-xwd"},
    {"sis",     "application/vnd.symbian.install"},
    {"sisx",    "application/vnd.symbian.install"},
    {"ipa",     "application/vnd.iphone"},
    {"apk",     "application/vnd.android.package-archive"},
    {"xap",     "application/x-silverlight-app"},
    {"wb1",     "application/x-wb1"},
    {"wb2",     "application/x-wb2"},
    {"wb3",     "application/x-wb3"},
    {"wiz",     "application/msword"},
    {"wk3",     "application/x-wk3"},
    {"wk4",     "application/x-wk4"},
    {"wkq",     "application/x-wkq"},
    {"wks",     "application/x-wks"},
    {"wmd",     "application/x-ms-wmd"},
    {"wmf",     "application/x-wmf"},
    {"ras",     "application/x-ras"},
    {"rat",     "application/rat-file"},
    {"rm",      "application/vnd.rn-realmedia"},
    {"rmf",     "application/vnd.adobe.rmf"},
    {"torrent", "application/x-bittorrent"},
    {"uin",     "application/x-icq"},
    {"vda",     "application/x-vda"},
    {"vdx",     "application/vnd.visio"},
    {"vpg",     "application/x-vpeg005"},
    {"vsd",     "application/vnd.visio"},
    {"vss",     "application/vnd.visio"},
    {"vst",     "application/vnd.visio"},
    {"vsw",     "application/vnd.visio"},
    {"vsx",     "application/vnd.visio"},
    {"vtx",     "application/vnd.visio"},
    {"mxp",     "application/x-mmxp"},
    {"nrf",     "application/x-nrf"},
    {"plt",     "application/x-plt"},
    {"rmp",     "application/vnd.rn-rn_music_package"},
    {"rms",     "application/vnd.rn-realmedia-secure"},
    {"rmvb",    "application/vnd.rn-realmedia-vbr"},
    {"rmx",     "application/vnd.rn-realsystem-rmx"},
    {"rnx",     "application/vnd.rn-realplayer"},
    {"rsml",    "application/vnd.rn-rsml"},
    {"rtf",     "application/msword"},
    {"sam",     "application/x-sam"},
    {"sat",     "application/x-sat"},
    {"sdp",     "application/sdp"},
    {"sdw",     "application/x-sdw"},
    {"sit",     "application/x-stuffit"},
    {"slb",     "application/x-slb"},
    {"sld",     "application/x-sld"},
    {"smi",     "application/smil"},
    {"smil",    "application/smil"},
    {"smk",     "application/x-smk"},
    {"spc",     "application/x-pkcs7-certificates"},
    {"spl",     "application/futuresplash"},
    {"ssm",     "application/streamingmedia"},
    {"sst",     "application/vnd.ms-pki.certstore"},
    {"stl",     "application/vnd.ms-pki.stl"},
    {"sty",     "application/x-sty"},
    {"swf",     "application/x-shockwave-flash"},
    {"tdf",     "application/x-tdf"},
    {"tg4",     "application/x-tg4"},
    {"tga",     "application/x-tga"},
    {"rmj",     "application/vnd.rn-realsystem-rmj"},
    {"fax",     "image/fax"},
    {"gif",     "image/gif"},
    {"ico",     "image/x-icon"},
    {"jpeg",    "image/jpeg"},
    {"jfif",    "image/jpeg"},
    {"jpe",     "image/jpeg"},
    {"jpg",     "image/jpeg"},
    {"net",     "image/pnetvue"},
    {"png",     "image/png"},
    {"rp",      "image/vnd.rn-realpix"},
    {"tif",     "image/tiff"},
    {"tiff",    "image/tiff"},
    {"wbmp",    "image/vnd.wap.wbmp"},
    {"323",     "text/h323"},
    {"asa",     "text/asa"},
    {"asp",     "text/asp"},
    {"biz",     "text/xml"},
    {"cml",     "text/xml"},
    {"css",     "text/css"},
    {"dcd",     "text/xml"},
    {"dtd",     "text/xml"},
    {"ent",     "text/xml"},
    {"fo",      "text/xml"},
    {"htc",     "text/x-component"},
    {"htm",     "text/html"},
    {"html",    "text/html"},
    {"htt",     "text/webviewhtml"},
    {"htx",     "text/html"},
    {"json",    "text/json"},
    {"jsp",     "text/html"},
    {"math",    "text/xml"},
    {"mml",     "text/xml"},
    {"mtx",     "text/xml"},
    {"odc",     "text/x-ms-odc"},
    {"plg",     "text/html"},
    {"r3t",     "text/vnd.rn-realtext3d"},
    {"rdf",     "text/xml"},
    {"rt",      "text/vnd.rn-realtext"},
    {"sol",     "text/plain"},
    {"sor",     "text/plain"},
    {"spp",     "text/xml"},
    {"stm",     "text/html"},
    {"svg",     "text/xml"},
    {"tld",     "text/xml"},
    {"tsd",     "text/xml"},
    {"txt",     "text/plain"},
    {"vxml",    "text/xml"},
    {"xdr",     "text/xml"},
    {"xhtml",   "text/html"},
    {"xml",     "text/xml"},
    {"xq",      "text/xml"},
    {"xql",     "text/xml"},
    {"xquery",  "text/xml"},
    {"xsd",     "text/xml"},
    {"xsl",     "text/xml"},
    {"xslt",    "text/xml"},
    {"uls",     "text/iuls"},
    {"vcf",     "text/x-vcard"},
    {"vml",     "text/xml"},
    {"wml",     "text/vnd.wap.wml"},
    {"wsdl",    "text/xml"},
    {"wsc",     "text/scriptlet"},
    {"acp",     "audio/x-mei-aac"},
    {"aif",     "audio/aiff"},
    {"aifc",    "audio/aiff"},
    {"aiff",    "audio/aiff"},
    {"au",      "audio/basic"},
    {"la1",     "audio/x-liquid-file"},
    {"lavs",    "audio/x-liquid-secure"},
    {"lmsff",   "audio/x-la-lms"},
    {"m3u",     "audio/mpegurl"},
    {"mid",     "audio/mid"},
    {"midi",    "audio/mid"},
    {"mnd",     "audio/x-musicnet-download"},
    {"mns",     "audio/x-musicnet-stream"},
    {"mp1",     "audio/mp1"},
    {"mp2",     "audio/mp2"},
    {"mp3",     "audio/mp3"},
    {"mpga",    "audio/rn-mpeg"},
    {"pls",     "audio/scpls"},
    {"ra",      "audio/vnd.rn-realaudio"},
    {"ram",     "audio/x-pn-realaudio"},
    {"rmi",     "audio/mid"},
    {"rmm",     "audio/x-pn-realaudio"},
    {"rpm",     "audio/x-pn-realaudio-plugin"},
    {"snd",     "audio/basic"},
    {"wav",     "audio/wav"},
    {"wax",     "audio/x-ms-wax"},
    {"wma",     "audio/x-ms-wma"},
    {"xpl",     "audio/scpls"},
    {"asf",     "video/x-ms-asf"},
    {"asx",     "video/x-ms-asf"},
    {"avi",     "video/avi"},
    {"IVF",     "video/x-ivf"},
    {"m1v",     "video/x-mpeg"},
    {"m2v",     "video/x-mpeg"},
    {"m4e",     "video/mp4"},
    {"movie",   "video/x-sgi-movie"},
    {"mp2v",    "video/mp4"},
    {"mp4",     "video/mp4"},
    {"mkv",     "video/webm"},
    {"mpa",     "video/x-mpg"},
    {"mpe",     "video/x-mpeg"},
    {"mpeg",    "video/mpg"},
    {"mpg",     "video/mpg"},
    {"mps",     "video/x-mpeg"},
    {"mpv",     "video/mpg"},
    {"mpv2",    "video/mp4"},
    {"rv",      "video/vnd.rn-realvideo"},
    {"wm",      "video/x-ms-wm"},
    {"wmv",     "video/x-ms-wmv"},
    {"wmx",     "video/x-ms-wmx"},
    {"wvx",     "video/x-ms-wvx"},
};

HttpMethod StringToHttpMethod(const std::string& m)
{
#define XX(num, name) \
    if (strcmp(#name, m.c_str()) == 0) \
    { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharsToHttpMethod(const char* m)
{
#define XX(num, name) \
    if (strncmp(#name, m, strlen(#name)) == 0) \
    { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

static const char* method_string[] =
{
#define XX(num, name) #name,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const std::string HttpMethodToString(const HttpMethod& method)
{
    size_t idx = (size_t)method;
    if (idx >= (sizeof(method_string) / sizeof(method_string[0])))
        return "<unknown>";
    return method_string[idx];
}

const std::string HttpStatusToString(const HttpStatus& status)
{
    switch(status)
    {
#define XX(code, name, msg) \
        case HttpStatus::name: \
            return #msg;
        HTTP_STATUS_MAP(XX);
#undef XX
        default:
            return "<unknown>";
    }
}


HttpRequest::HttpRequest(bool input, uint8_t version, bool close)
    :m_input(input)
    ,m_ssl(false)
    ,m_method(HttpMethod::GET)
    ,m_version(version)
    ,m_close(close)
    ,m_websocket(false)
    ,m_path("/")
{
}

std::shared_ptr<HttpResponse> HttpRequest::createResponse()
{
    HttpResponse::ptr rsp(new HttpResponse(!m_input, getVersion(), isClose()));
    return rsp;
}

void HttpRequest::initClose()
{
    std::string conn = getHeader("Connection");
    if (!conn.empty())
        m_close = !strcasecmp(conn.c_str(), "close") ? true : false;
}

#define PARSE_PARAM(str, m, flag, trim) \
    size_t pos = 0; \
    do \
    { \
        size_t last = pos; \
        pos = str.find('=', pos); \
        if (pos == std::string::npos) \
            break; \
        size_t key = pos; \
        pos = str.find(flag, pos); \
        m.insert(std::make_pair(trim(str.substr(last, key - last)), \
            str.substr(key + 1, pos - key - 1))); \
        if (pos == std::string::npos) \
            break; \
        pos++; \
    } while(1)

#define PARSE_PARAM_TRANSFER(str, m, flag, trim) \
    size_t pos = 0; \
    std::function<std::string(const std::string&, bool)> func = \
        m_input ? bifang::StringUtil::urlDecode : bifang::StringUtil::urlEncode; \
    do \
    { \
        size_t last = pos; \
        pos = str.find('=', pos); \
        if (pos == std::string::npos) \
            break; \
        size_t key = pos; \
        pos = str.find(flag, pos); \
        m.insert(std::make_pair(trim(func(str.substr(last, key - last), false)), \
            func(str.substr(key + 1, pos - key - 1), false))); \
        if (pos == std::string::npos) \
            break; \
        pos++; \
    } while(1)

void HttpRequest::initQuery(const std::string& query)
{
    m_queryMap.clear();
    PARSE_PARAM_TRANSFER(query, m_queryMap, '&', );
}

void HttpRequest::initContent()
{
    std::string content_type = getHeader("content-type");
    if (strcasestr(content_type.c_str(), "application/x-www-form-urlencoded") == nullptr)
        return;
    m_contentMap.clear();
    PARSE_PARAM(m_content, m_contentMap, '&', );
}

void HttpRequest::initCookie()
{
    std::string cookie = getHeader("cookie");
    if (cookie.empty())
        return;
    m_cookieMap.clear();
    PARSE_PARAM(cookie, m_cookieMap, ';', bifang::StringUtil::trim);
}

std::string HttpRequest::getHeader(const std::string& key,
                             const std::string& def) const
{
    auto it = m_headerMap.find(key);
    return it == m_headerMap.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string& key,
    const std::string& def) const
{
    std::string t_key = Transfer_URL(key, false);

    auto it = m_queryMap.find(t_key);
    if (it != m_queryMap.end())
        return it->second;

    auto iit = m_contentMap.find(key);
    if (iit != m_contentMap.end())
        return iit->second;

    return def;
}

std::string HttpRequest::getCookie(const std::string& key,
    const std::string& def) const
{
    auto it = m_cookieMap.find(key);
    return it == m_cookieMap.end() ? def : it->second;
}

std::string HttpRequest::toString()
{
    std::stringstream ss;
    
    ss << HttpMethodToString(m_method) << " "
       << m_path
       << (m_queryMap.empty() ? "" : "?");

    bool first_flag = true;
    for (auto& i : m_queryMap)
    {
        if (!first_flag)
            ss << "&";
        ss << i.first << "=" << i.second;
        first_flag = false;
    }

    ss << (m_fragment.empty() ? "" : "#")
       << m_fragment
       << " HTTP/"
       << ((uint32_t)(m_version >> 4))
       << "."
       << ((uint32_t)(m_version & 0x0F))
       << "\r\n";

    if (!m_websocket)
        ss << "Connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    for (auto& i : m_headerMap)
    {
        if (!m_websocket && strcasecmp(i.first.c_str(), "Connection") == 0)
            continue;
        if (strcasecmp(i.first.c_str(), "Cookie") == 0)
            continue;
        ss << i.first << ": " << i.second << "\r\n";
    }

    if (!m_cookieMap.empty())
    {
        first_flag = true;
        ss << "Cookie: ";
        for (auto& i : m_cookieMap)
        {
            if (!first_flag)
                ss << ";";
            ss << i.first << "=" << i.second;
            first_flag = false;
        }
        ss << "\r\n";
    }
    if (!m_content.empty() && getHeader("Content-Length").empty())
        ss << "Content-Length: " << m_content.size() << "\r\n";

    if (!m_content.empty())
        ss << "\r\n" << m_content;
    else
        ss << "\r\n";

    return ss.str();
}


HttpResponse::HttpResponse(bool input, uint8_t version, bool close)
    :m_input(input)
    ,m_status(HttpStatus::OK)
    ,m_version(version)
    ,m_close(close)
    ,m_websocket(false)
    ,m_gzip(false)
    ,m_rate(0)
    ,m_rate_after(0)
{
}

void HttpResponse::initClose()
{
    std::string conn = getHeader("connection");
    if (!conn.empty())
        m_close = !strcasecmp(conn.c_str(), "close") ? true : false;
}

void HttpResponse::setCookie(const std::string& key, const std::string& val,
                       time_t expired, const std::string& domain,
                       const std::string& path, bool secure)
{
    std::stringstream ss;
    ss << key << "=" << val;
    if (expired > 0)
        ss << ";expires=" << bifang::time_to_string(expired, "%a, %d %b %Y %H:%M:%S GMT", true);
    if (!domain.empty())
        ss << ";domain=" << domain;
    if (!path.empty())
        ss << ";path=" << path;
    if (secure)
        ss << ";secure";
    m_cookies.push_back(ss.str());
}

void HttpResponse::setConetentType(const std::string& filename)
{
#define XX(str, flag) \
    setHeader("Content-Type", str); \
    m_gzip = flag; \
    return

    size_t pos = filename.rfind(".");
    if (pos == std::string::npos || pos == filename.size() - 1)
    {
        XX("application/octet-stream", false);
    }
    else
    {
        //std::string subfix = filename.substr(pos + 1);
        //std::transform(subfix.begin(), subfix.end(), subfix.begin(), ::tolower);
        //auto it = ContentTypeList.find(subfix);
        //if (it == ContentTypeList.end())
        //{
            XX("text/plain; charset=utf-8", true);
        //}
        //else
        //{
        //    std::string type = it->second;
        //    if (!strncmp(type.c_str(), "text", 4))
        //    {
        //        XX(type + "; charset=utf-8", true);
        //    }
        //    else
        //    {
        //        XX(type, false);
        //    }
        //}
    }

#undef XX
}

bool HttpResponse::setLastModified(const std::string& filename,
                       HttpRequest::ptr request, const std::string& def)
{
    struct stat lst;
    std::string mtime;
    if (!def.empty())
        mtime = def;
    else if (!filename.empty() && FileUtil::__lstat(filename, &lst))
        mtime = time_to_string(lst.st_mtime, "%a, %d %b %Y %H:%M:%S GMT", true);
    else
        mtime = time_to_string(time(0), "%a, %d %b %Y %H:%M:%S GMT", true);

    setHeader("Last-Modified", mtime);
    if (request)
    {
        std::string ifmtime = request->getHeader("If-Modified-Since");
        if (!ifmtime.empty() && !strcasecmp(mtime.c_str(), ifmtime.c_str()))
        {
            m_status = HttpStatus::NOT_MODIFIED;
            //log_info << "no modified!!!";
            return true;
        }
    }
    return false;
}

bool HttpResponse::setETag(const std::string& filename, HttpRequest::ptr request)
{
    struct stat lst;
    if (!FileUtil::__lstat(filename, &lst))
        return false;

    char etag[36] = {0};
    sprintf(etag, "\"%x-%x\"", lst.st_mtime, lst.st_size);
    setHeader("ETag", etag);

    std::string ifnonematch = request->getHeader("If-None-Match");
    if (!ifnonematch.empty() && !strncasecmp(ifnonematch.c_str(), etag, ifnonematch.size()))
    {
        m_status = HttpStatus::NOT_MODIFIED;
        //log_info << "no modified!!!";
        return true;
    }
    return false;
}

void HttpResponse::setLocation(HttpRequest::ptr request, const std::string& path)
{
    std::string location;
    std::string host = request->getHeader("Host");
    if (host.empty())
        location = path;
    else
        location = (request->isSSL() ? "https://" : "http://") + host + path;

    location = StringUtil::urlEncode(location, true);
    setHeader("Location", location);
}

// static
/**
 * details:
 *     1. bytes=0-499            - 头500个字节
 *     2. bytes=-500             - 最后500个字节
 *     3. byets=500-             - 500字节以后的内容
 *     4. bytes=500-600, 601-999 - 同时指定多个范围(暂时不支持)
 */
static bool range_parse(const std::string& range, int64_t* start, int64_t* end)
{
    size_t head_pos = range.find("bytes=") + 6;
    if (head_pos == std::string::npos)
        return false;

    size_t delimit_pos = range.find("-");
    if (delimit_pos == std::string::npos)
        return false;

    std::string start_str = range.substr(head_pos, delimit_pos - head_pos);
    std::string end_str = range.substr(delimit_pos + 1);
    start_str = StringUtil::trim(start_str);
    end_str = StringUtil::trim(end_str);
    *start = -1, *end = -1;

    try
    {
        if (!start_str.empty())
            *start = boost::lexical_cast<int64_t>(start_str);
        if (!end_str.empty())
            *end = boost::lexical_cast<int64_t>(end_str);

        if (*start == -1 && *end == -1)
            return false;
        
        if (*start == -1 || *end == -1) // 第二和第三种情况, 允许的形式
            return true;

        if (*start > *end)
            return false;

        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}

void HttpResponse::setContent(const std::string& filename, HttpRequest::ptr request)
{
#define XX(name, start, end) \
    setHeader("Accept-Ranges", "bytes"); \
    setHeader("Content-Length", std::to_string(end - start + 1)); \
    m_filename = name; \
    m_start = start; \
    m_length = end - start + 1; \
    return

    struct stat lst;
    if (!FileUtil::__lstat(filename, &lst))
    {
        m_status = HttpStatus::NOT_FOUND;
        return;
    }

    size_t file_size = lst.st_size;
    if (m_gzip) // 需要gzip压缩的文件
    {
        std::ifstream ifs(filename);
        if (!ifs)
        {
            m_status = HttpStatus::NOT_FOUND;
            return;
        }
        m_content.resize(file_size);
        ifs.read(&m_content[0], file_size);
        return;
    }

    std::string range;
    std::string if_range;
    std::string etag;
    char content_range[60] = {0};

    range = request->getHeader("Range");
    if (range.size() < 7 || range.find("bytes=") == std::string::npos)
    {
        XX(filename, 0, file_size - 1);
    }

    if_range = request->getHeader("If-Range");
    if (!if_range.empty())
    {
        etag = getHeader("ETag");
        if (if_range != etag)
        {
            XX(filename, 0, file_size - 1);
        }
    }

    int64_t start, end;
    if (!range_parse(range, &start, &end))
    {
        m_status = HttpStatus::RANGE_NOT_SATISFIABLE;
        sprintf(content_range, "bytes */%ld", file_size);
        setHeader("Content-Range", content_range);
        XX(filename, 0, file_size - 1);
    }

    log_debug << "start: " << start << ", end: " << end;
    if (start == -1)
    {
        start = file_size - 1 - end;
        if (start < 0)
            start = 0;
        end = file_size - 1;
    }
    else if (end == -1)
        end = file_size - 1;

    if (start > (int64_t)(file_size - 1))
        start = file_size - 1;
    if (end > (int64_t)(file_size - 1))
        end = file_size - 1;

    m_status = HttpStatus::PARTIAL_CONTENT;
    sprintf(content_range, "bytes %ld-%ld/%ld", start, end, file_size);
    setHeader("Content-Range", content_range);
    XX(filename, start, end);

#undef XX
}

std::string HttpResponse::toString()
{
    std::stringstream ss;

    ss << "HTTP/"
       << ((uint32_t)(m_version >> 4))
       << "."
       << ((uint32_t)(m_version & 0x0F))
       << " "
       << (uint32_t)m_status
       << " "
       << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
       << "\r\n";

    m_headerMap["Date"] = time_to_string(time(0), "%a, %d %b %Y %H:%M:%S GMT", true);

    if (m_status == HttpStatus::FORBIDDEN)
        set_403_content();
    else if (m_status == HttpStatus::NOT_FOUND)
        set_404_content();

    for (auto& i : m_headerMap)
    {
        if (!m_websocket && strcasecmp(i.first.c_str(), "Connection") == 0)
            continue;
        ss << i.first << ": " << i.second << "\r\n";
    }
    for (auto& i : m_cookies)
        ss << "Set-Cookie: " << i << "\r\n";
    if (!m_websocket)
        ss << "Connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    if (!m_filename.empty()) // 大文件
        ss << "\r\n";
    else
    {
        if (m_gzip && m_content.size() > m_gzip_min_length)
        {
            auto zs = ZlibStream::create(true, ZlibStream::GZIP, m_gzip_comp_level);
            zs->write(m_content.c_str(), m_content.size());
            zs->flush();
            zs->toString().swap(m_content);
            ss << "Content-Encoding: gzip\r\n";
        }

        ss << "Content-Length: " << m_content.size() << "\r\n\r\n" << m_content;
    }

    return ss.str();
}

void HttpResponse::set_403_content()
{
    static const char* head = 
        "<html>\n"
            "<head>\n"
                "<title>403 Forbidden</title>\n"
            "</head>\n"
            "<body>\n"
                "<center><h1>403 Forbidden</h1></center>\n";

    static const char* tail = 
            "</body>\n"
        "</html>\n";

    std::string name = getHeader("Server");
    if (name.empty())
        name = SYS_VERSION;
    name = "<hr><center>" + name + "</center>\n";

    m_content = head + name + tail;

    setHeader("Content-Type", "text/html; charset=utf-8");
    m_filename.clear();
}

void HttpResponse::set_404_content()
{
    static const char* head = 
        "<html>\n"
            "<head>\n"
                "<title>404 Not Found</title>\n"
            "</head>\n"
            "<body>\n"
                "<center><h1>404 Not Found</h1></center>\n";

    static const char* tail = 
            "</body>\n"
        "</html>\n";

    std::string name = getHeader("Server");
    if (name.empty())
        name = SYS_VERSION;
    name = "<hr><center>" + name + "</center>\n";

    m_content = head + name + tail;

    setHeader("Content-Type", "text/html; charset=utf-8");
    m_filename.clear();
}


// global
std::ostream& operator<<(std::ostream& os, const HttpRequest::ptr& req)
{
    os << req->toString();
    return os;
}
// global
std::ostream& operator<<(std::ostream& os, const HttpResponse::ptr& rsp)
{
    os << rsp->toString();
    return os;
}

}

}
