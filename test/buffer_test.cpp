/**
 * brief: buffer简单测试
 */
#include "bifang.h"

RootLogger();

void test()
{
    bifang::Srand();
#define XX(type, len, write_fun, read_fun, base_len) \
    {\
        std::vector<type> vec; \
        for (size_t i = 0; i < len; i++) \
            vec.push_back(rand()); \
        bifang::Buffer::ptr buffer(new bifang::Buffer(base_len)); \
        for (auto& i : vec) \
            buffer->write_fun(i); \
        buffer->setPosition(0); \
        for (size_t i = 0; i < vec.size(); i++) \
        { \
            type v = buffer->read_fun((type)1); \
            ASSERT(v == vec[i]); \
        } \
        ASSERT(buffer->getReadSize() == 0); \
        log_info << #write_fun "/" #read_fun " (" #type ") len=" << len \
            << " base_len=" << base_len << " size=" << buffer->getSize(); \
    }

    XX(int8_t,   1000, writeInt,       readInt,       100);
    XX(uint8_t,  1000, writeInt,       readInt,       100);
    XX(int16_t,  1000, writeInt,       readInt,       100);
    XX(uint16_t, 1000, writeInt,       readInt,       100);
    XX(int32_t,  1000, writeInt,       readInt,       100);
    XX(uint32_t, 1000, writeInt,       readInt,       100);
    XX(int64_t,  1000, writeInt,       readInt,       100);
    XX(uint64_t, 1000, writeInt,       readInt,       100);
    XX(float,    1000, writeFloat,     readFloat,     100);
    XX(double,   1000, writeDouble,    readDouble,    100);
    XX(int32_t,  1000, writeVarint32,  readVarint32,  100);
    XX(uint32_t, 1000, writeVaruint32, readVaruint32, 100);
    XX(int64_t,  1000, writeVarint64,  readVarint64,  100);
    XX(uint64_t, 1000, writeVaruint64, readVaruint64, 100);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) \
    {\
        std::vector<type> vec; \
        for (size_t i = 0; i < len; i++) \
            vec.push_back(rand()); \
        bifang::Buffer::ptr buffer(new bifang::Buffer(base_len)); \
        for (auto& i : vec) \
            buffer->write_fun(i); \
        buffer->setPosition(0); \
        for (size_t i = 0; i < vec.size(); i++) \
        { \
            type v = buffer->read_fun((type)1); \
            ASSERT(v == vec[i]); \
        } \
        ASSERT(buffer->getReadSize() == 0); \
        log_info << #write_fun "/" #read_fun " (" #type ") len=" << len \
            << " base_len=" << base_len << " size=" << buffer->getSize(); \
        buffer->setPosition(0); \
        ASSERT(buffer->writeToFile("./logs/" #type "_" #len "-" #read_fun ".txt")); \
        bifang::Buffer::ptr buffer_(new bifang::Buffer(base_len * 2)); \
        ASSERT(buffer_->readFromFile("./logs/" #type "_" #len "-" #read_fun ".txt")); \
        buffer_->setPosition(0); \
        ASSERT(buffer->toString() == buffer_->toString()); \
        ASSERT(buffer->getPosition() == 0); \
        ASSERT(buffer_->getPosition() == 0); \
    }

    XX(int8_t,   1000, writeInt,    readInt,    1);
    XX(uint8_t,  1000, writeInt,    readInt,    1);
    XX(int16_t,  1000, writeInt,    readInt,    1);
    XX(uint16_t, 1000, writeInt,    readInt,    1);
    XX(int32_t,  1000, writeInt,    readInt,    1);
    XX(uint32_t, 1000, writeInt,    readInt,    1);
    XX(int64_t,  1000, writeInt,    readInt,    1);
    XX(uint64_t, 1000, writeInt,    readInt,    1);
    XX(float,    1000, writeFloat,  readFloat,  1);
    XX(double,   1000, writeDouble, readDouble, 1);
    XX(int32_t,  1000, writeVarint32,  readVarint32,  10);
    XX(uint32_t, 1000, writeVaruint32, readVaruint32, 10);
    XX(int64_t,  1000, writeVarint64,  readVarint64,  10);
    XX(uint64_t, 1000, writeVaruint64, readVaruint64, 10);
#undef XX
}

int main(int argc, char* argv[])
{
    test();

    return 0;
}
