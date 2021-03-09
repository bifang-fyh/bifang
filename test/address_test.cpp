/**
 * brief: Address简单测试
 */
#include "bifang.h"

RootLogger();

void GetAddrInfo_test(int family)
{
    log_info << "GetAddrInfo_test begin";
    std::vector<bifang::Address::ptr> addresses;
    std::string addr_str;

    addr_str = "localhost";
    log_info << "get " + addr_str + " info";
    if (!bifang::Address::getAddrInfo(addresses, addr_str, family))
        log_error << "get " + addr_str + " info fail";
    for (size_t i = 0; i < addresses.size(); i++)
        log_info << i << " - " << addresses[i]->toString();

    addresses.clear();
    addr_str = "www.baidu.com";
    log_info << "get " + addr_str + " info";
    if (!bifang::Address::getAddrInfo(addresses, addr_str, family))
        log_error << "get " + addr_str + " info fail";
    for (size_t i = 0; i < addresses.size(); i++)
        log_info << i << " - " << addresses[i]->toString();

    addr_str = "localhost:4444";
    log_info << "get any " + addr_str + " info";
    for (size_t i = 0; i < 7; i++)
    {
        auto addr = bifang::Address::getAddrInfo(addr_str, family);
        if (addr)
            log_info << *addr;
        else
            log_error << "get any " + addr_str + " info error";
    }
    log_info << "GetAddrInfo_test end";
}

void GetInterface_test(int family)
{
    log_info << "GetInterface_test begin";
    std::multimap<std::string, std::pair<bifang::Address::ptr, uint32_t> > addresses;

    bool v = bifang::Address::getInterface(addresses, family);
    if (!v)
    {
        log_error << "getInterface fail";
        return;
    }

    for (auto& i: addresses)
    {
        log_info << i.first << " - " << i.second.first->toString()
            << " - " << i.second.second;
    }
    log_info << "GetInterface_test end";
}

void ipv4_test()
{
    log_info << "ipv4_test begin";
    auto addr = bifang::Address::create("127.0.2.22");
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("www.baidu.com");
    if (addr)
        log_info << addr->toString();
    log_info << "ipv4_test end";
}

void ipv6_test()
{
    log_info << "ipv6_test begin";

    auto addr = bifang::Address::create("fe80:4c00:4c00:5741:4c00:4c00:4c00:824b");
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("fe80::4c00:5741:824b", 44);
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("::4c00:5741:824b", 432);
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("fe80:4c00:4c00::", 9999);
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("fe80::824b");
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("::");
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("::824b");
    if (addr)
        log_info << addr->toString();

    addr = bifang::Address::create("824b::");
    if (addr)
        log_info << addr->toString();

    log_info << "ipv6_test end";
}


int main(int argc, char* argv[])
{
    GetAddrInfo_test(AF_INET);
    std::cout << std::endl;

    GetInterface_test(AF_INET);
    std::cout << std::endl;

    ipv4_test();
    std::cout << std::endl;

    ipv6_test();

    return 0;
}
