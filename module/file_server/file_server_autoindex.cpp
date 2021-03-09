#include "file_server_autoindex.h"


namespace file_server
{

RootLogger();

#define CRLF    "\r\n"

AutoIndex::AutoIndex(const std::string& path)
    :m_path(path)
{
}

bool AutoIndex::init()
{
    return bifang::FileUtil::GetDirInformation(m_path, m_info);
}

std::string AutoIndex::toString(const std::string& label)
{
    std::string ret;
    ret += "<html>" CRLF "<head><title>Index of " + label + "</title></head>" CRLF;
    ret += "<script>" CRLF
           "    var i = 0;" CRLF
           "    function buttom(data) {" CRLF
           "        i++;" CRLF
           "        setTimeout(function() {" CRLF
           "            if (i == 1) {" CRLF
           "                window.location.href = data.title + \"?flag=0\"" CRLF
           "            }" CRLF
           "            i = 0;" CRLF
           "        }, 300);" CRLF
           "        if (i > 1) {" CRLF
           "            window.location.href = data.title + \"?flag=1\"" CRLF
           "            i = 0;" CRLF
           "            return;" CRLF
           "        }" CRLF
           "        return false;" CRLF
           "    }" CRLF
           "</script>" CRLF;
    ret += "<body>" CRLF "<h1>Index of " + label + "</h1><hr>";

    if (label == "/") // 根目录
        ret += "<pre><a href=\"./\">./</a>" CRLF;
    else
        ret += "<pre><a href=\"../\">../</a>" CRLF;

    for (auto& info : m_info)
    {
        if (info.name[info.name.size() - 1] == '/')
            ret += "<a href=\"" + info.name + "\">";
        else
            ret += "<a title=\"" + info.name + "\" href=\"javascript:void(0)\" onclick=\"buttom(this)\">";

        int compensation = bifang::StringUtil::countCompensation(info.name, 2) + MAX_NAME_LEN - info.name.size();
        if (compensation >= 0)
        {
            ret += info.name + "</a>";
            ret += std::string(compensation, ' ');
        }
        else
        {
            auto cur_len = bifang::StringUtil::countOriginalLength(info.name, MAX_NAME_LEN - 3, 2);
            ret += info.name.substr(0, cur_len[0]) + "...</a>";
            ret += std::string(cur_len[1], ' ');
        }

        ret += bifang::time_to_string(info.mtime, " %d-%b-%Y %H:%M ");

        if (info.name[info.name.size() - 1] == '/')
        {
            ret += "     -";
        }
        else
        {
            off_t length = info.size;
            char scale;
            off_t size;

            if (length > (off_t)(1024 * 1024 * 1024 - 1))
            {
                size = length / (1024 * 1024 * 1024);
                if ((length % (1024 * 1024 * 1024)) > (1024 * 1024 * 1024 / 2 - 1))
                    size++;
                scale = 'G';
            }
            else if (length > (off_t)(1024 * 1024 - 1))
            {
                size = length / (1024 * 1024);
                if ((length % (1024 * 1024)) > (1024 * 1024 / 2 - 1))
                    size++;
                scale = 'M';
            }
            else if (length > (off_t)(9999))
            {
                size = length / 1024;
                if (length % 1024 > 511)
                    size++;
                scale = 'K';
            }
            else
            {
                size = length;
                scale = '\0';
            }

            std::string tmp;
            tmp.resize(6);
            if (scale)
                sprintf(&tmp[0], "%5d%c", size, scale);
            else
                sprintf(&tmp[0], "%6d", size);

            ret += tmp;
        }

        ret += CRLF;
    }

    ret += "</pre><hr>" CRLF "</body>" CRLF "</html>" CRLF;

    return ret;
}

}
