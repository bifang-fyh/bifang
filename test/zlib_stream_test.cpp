/**
 * brief: zlib简单测试
 */
#include "bifang.h"


void deflate_test()
{
    std::cout << "----------deflate begin----------" << std::endl;
    std::string data = bifang::StringUtil::randomString(102400);

    auto deflate_compress = bifang::ZlibStream::create(true, bifang::ZlibStream::DEFLATE);
    std::cout << "compress: " << (!deflate_compress->write(data.c_str(), data.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!deflate_compress->flush() ? "successful" : "fail")
              << std::endl;

    auto str = deflate_compress->toString();
    auto deflate_uncompress = bifang::ZlibStream::create(false, bifang::ZlibStream::DEFLATE);
    std::cout << "uncompress: " << (!deflate_uncompress->write(str.c_str(), str.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!deflate_uncompress->flush() ? "successful" : "fail")
              << std::endl;

    auto uncompress_str = deflate_uncompress->toString();
    std::cout << "deflate_test: " << (data == uncompress_str ? "successful" : "fail")
              << ", origin_size: " << data.size()
              << ", uncompress_size: " << uncompress_str.size()
              << std::endl;
    std::cout << "----------deflate end----------" << std::endl;
}

void zlib_test()
{
    std::cout << "----------zlib begin----------" << std::endl;
    std::string data = bifang::StringUtil::randomString(102400);

    auto zlib_compress = bifang::ZlibStream::create(true, bifang::ZlibStream::ZLIB);
    std::cout << "compress: " << (!zlib_compress->write(data.c_str(), data.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!zlib_compress->flush() ? "successful" : "fail")
              << std::endl;

    auto str = zlib_compress->toString();
    auto zlib_uncompress = bifang::ZlibStream::create(false, bifang::ZlibStream::ZLIB);
    std::cout << "uncompress: " << (!zlib_uncompress->write(str.c_str(), str.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!zlib_uncompress->flush() ? "successful" : "fail")
              << std::endl;

    auto uncompress_str = zlib_uncompress->toString();
    std::cout << "zlib_test: " << (data == uncompress_str ? "successful" : "fail")
              << ", origin_size: " << data.size()
              << ", uncompress_size: " << uncompress_str.size()
              << std::endl;
    std::cout << "----------zlib end----------" << std::endl;
}

void gzip_test()
{
    std::cout << "----------gzip begin----------" << std::endl;
    std::string data = bifang::StringUtil::randomString(102400);

    auto gzip_compress = bifang::ZlibStream::create(true, bifang::ZlibStream::GZIP);
    std::cout << "compress: " << (!gzip_compress->write(data.c_str(), data.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!gzip_compress->flush() ? "successful" : "fail")
              << std::endl;

    auto str = gzip_compress->toString();
    auto gzip_uncompress = bifang::ZlibStream::create(false, bifang::ZlibStream::GZIP);
    std::cout << "uncompress: " << (!gzip_uncompress->write(str.c_str(), str.size()) ? "successful" : "fail")
              << std::endl;
    std::cout << "flush: " << (!gzip_uncompress->flush() ? "successful" : "fail")
              << std::endl;

    auto uncompress_str = gzip_uncompress->toString();
    std::cout << "gzip_test: " << (data == uncompress_str ? "successful" : "fail")
              << ", origin_size: " << data.size()
              << ", uncompress_size: " << uncompress_str.size()
              << std::endl;
    std::cout << "----------gzip end----------" << std::endl;
}

int main(int argc, char* argv[])
{
    srand(time(0));
    deflate_test();
    std::cout << std::endl << std::endl;
    zlib_test();
    std::cout << std::endl << std::endl;
    gzip_test();

    return 0;
}
