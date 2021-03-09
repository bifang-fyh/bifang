#include "Assert.h"

namespace bifang
{

SystemLogger();

// static
static std::string demangle(const char* str)
{
    size_t size = 0;
    int status = 0;
    std::string sstr;
    sstr.resize(256);

    if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &sstr[0]))
    {
        char* tmp = abi::__cxa_demangle(&sstr[0], nullptr, &size, &status);
        if (tmp)
        {
            std::string result(tmp);
            free(tmp); 
            return result;
        }
    }

    if (1 == sscanf(str, "%255s", &sstr[0]))
        return sstr;

    return str;
}

std::string backtrace(int size, int skip, const std::string& prefix)
{
    std::vector<std::string> bt;
    void** array = (void **)malloc(sizeof(void*) * size);
    size_t s = ::backtrace(array, size);
    char** str = ::backtrace_symbols(array, s);
    std::stringstream ss;

    if (str == nullptr)
    {
        log_error << "backtrace_symbols fail";
        goto good;
    }
    for (size_t i = skip; i < s; i++)
        bt.push_back(demangle(str[i]));
    free(str);
    free(array);

    for (auto& b : bt)
        ss << prefix << b << std::endl;

good:
    return ss.str();
}

}
