/**
 * brief: B编码简单测试
 */
#include "bifang.h"

RootLogger();

using namespace bifang;

void write_test(const std::string& filename)
{
    BEncode::Value root(BEncode::Value::BCODE_DICTIONARY);
    root["creation date"] = BEncode::Value(1619317790);
    root["comment"] = BEncode::Value("Bencode test");
    root["created by"] = BEncode::Value("bifang");
    BEncode::Value info(BEncode::Value::BCODE_DICTIONARY);
    info["piece length"] = BEncode::Value(1048576);
    info["name"] = BEncode::Value("Bencode-cpp");
    info["pieces"] = BEncode::Value("64yytgyjhghj765r7u76ujt43tyehyy5eyyht");
    BEncode::Value files(BEncode::Value::BCODE_LIST);
    for (size_t i = 0; i < 4; i++)
    {
        BEncode::Value file(BEncode::Value::BCODE_DICTIONARY);
        file["length"] = BEncode::Value(262144);
        BEncode::Value l(BEncode::Value::BCODE_LIST);
        l.append(BEncode::Value("src"));
        l.append(BEncode::Value("cpp"));
        file["path"] = l;
        files.append(file);
    }
    info["files"] = files;
    root["info"] = info;

    if (BEncode::encode_to_file(filename, &root))
        std::cout << "write to file(" << filename << ") successful" << std::endl;
    else
        std::cout << "write to file(" << filename << ") fail" << std::endl;
}

void read_test(const std::string& filename)
{
    BEncode::Value root;
    int ret = BEncode::decode_from_file(filename, &root);
    if (ret == 0)
    {
        std::cout << "read from file(" << filename << ") successful" << std::endl;
        std::cout << BEncode::print(&root) << std::endl;
    }
    else
        std::cout << "read from file(" << filename << ") fail" << std::endl;
}

int main(int argc, char* argv[])
{
    std::string filename = "logs/test1";
    write_test(filename);
    read_test(filename);

    return 0;
}
