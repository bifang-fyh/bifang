/**
 * brief: 自定义数据缓冲区
 */
#ifndef __BIFANG_BUFFER_H
#define __BIFANG_BUFFER_H

#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <string.h>
#include <type_traits>
#include <sys/types.h>
#include <sys/socket.h>

namespace bifang
{

class Buffer
{
public:
    typedef std::shared_ptr<Buffer> ptr;

    /**
     * brief: 存储节点
     */
    struct Node
    {
        /**
         * brief: 构造指定大小的内存块
         * param: s 内存块字节数
         */
        Node(size_t s)
            :ptr(new char[s])
            ,next(nullptr)
            ,size(s)
        {
        }

        /**
         * brief: 析构函数,释放内存
         */
        ~Node()
        {
            if (ptr)
                delete[] ptr;
        }

        // 内存块地址指针
        char* ptr;
        // 下一个内存块地址
        Node* next;
        // 内存块大小
        size_t size;
    };

    /**
     * brief: 使用指定长度的内存块构造Buffer
     * param: base_size 内存块大小
     */
    Buffer(size_t base_size = 4096);

    /**
     * brief: 析构函数
     */
    ~Buffer();

    /**
     * brief: 返回内存块的大小
     */
    size_t getBaseSize() const { return m_baseSize; }
    /**
     * brief: 返回数据的长度
     */
    size_t getSize() const { return m_size; }
    /**
     * brief: 返回可读取数据大小
     */
    size_t getReadSize() const { return m_size - m_position; }
    /**
     * brief: 返回Buffer当前位置
     */
    size_t getPosition() const { return m_position; }

    /**
     * brief: 设置Buffer当前位置
     * post: 如果 m_position > m_size, 则 m_size = m_position
     * exception: 如果 m_position > m_capacity, 则抛出 std::out_of_range
     */
    void setPosition(size_t v, bool movement = true);

    /**
     * brief: 初始化Buffer(不清空释放已有内存)
     * post: m_position = 0, m_size = 0, m_cur = m_root
     */
    void init();

    /**
     * brief: 清空Buffer
     * post: m_position = 0, m_size = 0
     */
    void clear();

    /**
     * brief: 写入size长度的数据
     * param: buf 内存缓存指针
     *        size 数据大小
     * post: m_position += size, 如果 m_position > m_size, 则 m_size = m_position
     */
    void write(const void* buf, size_t size);

    /**
     * brief: 读取size长度的数据(读取数据且改变缓冲器指针指向)
     * param: buf 内存缓存指针
     *        size 数据大小
     * post: m_position += size, 如果m_position > m_size, 则 m_size = m_position
     * exception: 如果 GetReadSize() < size, 则抛出 std::out_of_range
     */
    void read(void* buf, size_t size);

    /**
     * brief: 读取size长度的数据(单纯读取数据但是不改变缓冲器指针指向)
     * param: buf 内存缓存指针
     *        size 数据大小
     *        position 读取开始位置
     * exception: 如果 (m_size - position) < size, 则抛出 std::out_of_range
     */
    void read(void* buf, size_t size, size_t position) const;

    /**
     * brief: 写入整形或布尔类型的数据(自动识别类型)
     *        (支持类型: int8_t uint8_t int16_t uint16_t int32_t uint32_t
                         int64_t uint64_t)
     * post: m_position += sizeof(value)
     *       如果 m_position > m_size, 则 m_size = m_position
     */
    template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
    void writeInt(T value)
    {
        write(&value, sizeof(value));
    }
    /**
     * brief: 写入float类型的数据
     * post: m_position += sizeof(value)
     *       如果 m_position > m_size, 则 m_size = m_position
     */
    void writeFloat(float value);
    /**
     * brief: 写入double类型的数据
     * post: m_position += sizeof(value)
     *       如果 m_position > m_size, 则 m_size = m_position
     */
    void writeDouble(double value);
    /**
     * brief: 写入有符号Varint32类型的数据
     * post: m_position += 实际占用内存(1 ~ 5)
     *       如果 m_position > m_size 则 m_size = m_position
     */
    void writeVarint32(int32_t value);
    /**
     * brief: 写入无符号Varint32类型的数据
     * post: m_position += 实际占用内存(1 ~ 5)
     *       如果 m_position > m_size 则 m_size = m_position
     */
    void writeVaruint32(uint32_t value);
    /**
     * brief: 写入有符号Varint64类型的数据
     * post: m_position += 实际占用内存(1 ~ 10)
     *       如果 m_position > m_size 则 m_size = m_position
     */
    void writeVarint64(int64_t value);
    /**
     * brief: 写入无符号Varint64类型的数据
     * post: m_position += 实际占用内存(1 ~ 10)
     *       如果 m_position > m_size 则 m_size = m_position
     */
    void writeVaruint64(uint64_t value);
    /**
     * brief: 写入std::string类型的数据
     * post: m_position += value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeString(const std::string& value);

    /**
     * brief: 读取整形或布尔类型的数据(自动识别类型)
     *        (支持类型: int8_t uint8_t int16_t uint16_t int32_t uint32_t
                         int64_t uint64_t)
     * param: value 为了显示地触发模板识别功能
     * pre: GetReadSize() >= sizeof(int8_t)
     * post: m_position += sizeof(int8_t);
     * exception: 如果 GetReadSize() < sizeof(int8_t), 则抛出 std::out_of_range
     */
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    readInt(const T& value)
    {
        T v;
        read(&v, sizeof(v));

        return v;
    }
    /**
     * brief: 读取float类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= sizeof(float)
     * post: m_position += sizeof(float);
     * exception: 如果 GetReadSize() < sizeof(float), 则抛出 std::out_of_range
     */
    float readFloat(const float& value = 0.0);
    /**
     * brief: 读取double类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= sizeof(double)
     * post: m_position += sizeof(double);
     * exception: 如果 GetReadSize() < sizeof(double), 则抛出 std::out_of_range
     */
    double readDouble(const double& value = 0.0);
    /**
     * brief: 读取有符号Varint32类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= 有符号Varint32实际占用内存
     * post: m_position += 有符号Varint32实际占用内存
     * exception: 如果 GetReadSize() < 有符号Varint32实际占用内存,
     *            则抛出 std::out_of_range
     */
    int32_t readVarint32(const int32_t& value = 0);
    /**
     * brief: 读取无符号Varint32类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= 无符号Varint32实际占用内存
     * post: m_position += 无符号Varint32实际占用内存
     * exception: 如果 GetReadSize() < 无符号Varint32实际占用内存,
     *            则抛出 std::out_of_range
     */
    uint32_t readVaruint32(const uint32_t& value = 0);
    /**
     * brief: 读取有符号Varint64类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= 有符号Varint64实际占用内存
     * post: m_position += 有符号Varint64实际占用内存
     * exception: 如果 GetReadSize() < 有符号Varint64实际占用内存,
     *            则抛出 std::out_of_range
     */
    int64_t readVarint64(const int64_t& value = 0);
    /**
     * brief: 读取无符号Varint64类型的数据
     * param: value 为了好看, 没卵用
     * pre: GetReadSize() >= 无符号Varint64实际占用内存
     * post: m_position += 无符号Varint64实际占用内存
     * exception: 如果 GetReadSize() < 无符号Varint64实际占用内存,
     *            则抛出 std::out_of_range
     */
    uint64_t readVaruint64(const uint64_t& value = 0);

    /**
     * brief: 把Buffer的数据写入到文件中
     * param: filename 文件名
     */
    bool writeToFile(const std::string& filename) const;

    /**
     * brief: 从文件中读取数据
     * param: filename 文件名
     */
    bool readFromFile(const std::string& filename);

    /**
     * brief: 将Buffer里面的数据[m_position, m_size)转成std::string
     */
    std::string toString() const;

    /**
     * brief: 将Buffer里面的数据[m_position, m_size)转成16进制的
     *        std::string(格式:FF FF FF)
     */
    std::string toHexString() const;

    /**
     * brief: 获取可读取的缓存, 保存成iovec数组
     * param: buffers 保存可读取数据的iovec数组
     *        len 读取数据的长度, 如果len > GetReadSize(), 则 len = GetReadSize()
     * return: 返回实际数据的长度
     */
    uint64_t getReadBuffers(std::vector<iovec>& buffers,
                 uint64_t len = ~0ull) const;

    /**
     * brief: 获取可写入的缓存, 保存成iovec数组
     * param: buffers 保存可写入的内存的iovec数组
     *        len 写入的长度
     * return: 返回实际的长度
     * post: 如果(m_position + len) > m_capacity, 则 m_capacity 扩容
     *       N个节点以容纳len长度
     */
    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);

private:
    /**
     * brief: 扩容Buffer, 使其可以容纳size个数据(如果原本可以可以容纳, 则不扩容)
     */
    void addCapacity(size_t size);

    /**
     * brief: 获取当前的可写入容量
     */
    size_t getCapacity() const { return m_capacity - m_position; }

private:
    // 内存块的大小
    size_t m_baseSize;
    // 当前操作位置
    size_t m_position;
    // 当前的总容量
    size_t m_capacity;
    // 当前数据的大小
    size_t m_size;
    // 第一个内存块指针
    Node* m_root;
    // 当前操作的内存块指针
    Node* m_cur;
};

}

#endif /*__BIFANG_BUFFER_H*/
