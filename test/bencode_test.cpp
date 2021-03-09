/**
 * brief: B编码简单测试
 */
#include "bifang.h"

RootLogger();

void test()
{
#if 0
    std::vector<std::string> files;
    bifang::FileUtil::ListAllFile(files, "./configs", ".tor");

    for (auto& i : files)
    {
        bifang::BCodeValue root;
        log_info << "read(" + i + "): " << bifang::BEncoding::decode_from_file(i, &root);
    
        std::string name = "./configs/tr_" + bifang::StringUtil::findSubPath(i, "./configs/") + "r";
        log_info << "write(" + name + "): " << bifang::BEncoding::encode_to_file(name, &root);
    }
#endif

#if 0
    std::string r = bifang::StringUtil::randomString(40);
    std::string sha1 = bifang::CryptUtil::sha1sum(r);
    std::string hex = bifang::StringUtil::toHexString(sha1);
    std::cout << hex << std::endl;
    std::string str = "aa";
    str.append(1, 0);
    str.append(1, 0);
    str.append(1, 0);
    std::cout << "str=" << str << ", size=" << str.size() << std::endl;
#endif

#if 0
    std::stringstream ss;
    std::string str = "aa";
    str.append(1, 0);
    str.append(1, 0);
    str.append(1, 0);
    std::cout << "str=" << str << ", size=" << str.size() << std::endl;
    ss << str;
    std::cout << "ss=" << ss.str() << ", size=" << ss.str().size() << std::endl;
#endif
    
#if 1
    std::string str;
    str.resize(10);
    str[0] = 0;
    str[1] = 0;
    str[2] = 0;
    str[3] = 'a';
    std::cout << "str=" << str << ", size=" << str.size() << std::endl;

    std::string str1;
    str1.resize(10);
    str1[0] = 0;
    str1[1] = 0;
    str1[2] = 0;
    str1[3] = 'a';
    str1[4] = 0;
    
    std::cout << "str1=" << str1 << ", size=" << str1.size() << std::endl;

    std::cout << (str1 == str) << std::endl;;
#endif
}

int main(int argc, char* argv[])
{
    test();

    return 0;
}
