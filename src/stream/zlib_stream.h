#ifndef __BIFANG_ZLIB_STREAM_H
#define __BIFANG_ZLIB_STREAM_H

#include <memory>
#include <string>
#include <vector>
#include <zlib.h>

#include "stream.h"

namespace bifang
{

/**
 * brief: Zlib流
 */
class ZlibStream : public Stream
{
public:
    typedef std::shared_ptr<ZlibStream> ptr;

    /**
     * brief: 压缩类型
     */
    enum Type
    {
        DEFLATE = 0,
        ZLIB,
        GZIP
    };
    /**
     * brief: 压缩策略
     */
    enum Strategy
    {
        DEFAULT = Z_DEFAULT_STRATEGY,
        FILTERED = Z_FILTERED,
        HUFFMAN = Z_HUFFMAN_ONLY,
        RLE = Z_RLE,
        FIXED = Z_FIXED
    };

    /**
     * brief: 构造函数
     * param: encode true表示压缩, false表示解压
     *        buff_size 压缩块大小
     */
    ZlibStream(bool encode, uint32_t buff_size = 4096);

    ~ZlibStream();

    /**
     * brief: 刷新压缩流
     * return: 0    - 刷新成功
     *         其他 - 刷新失败
     */
    int flush();

    /**
     * brief: 压缩/解压数据
     * param: buffer 待压缩/解压数据的内存
     *        length 待压缩/解压数据的内存长度
     * return: 0    - 压缩/解压成功
     *         其他 - 压缩/解压失败
     */
    virtual int write(const void* buffer, size_t length) override;

    /**
     * brief: 压缩/解压数据
     * param: buffer 待压缩/解压数据的Buffer
     *        length 待压缩/解压数据的内存长度
     * return: 0    - 压缩/解压成功
     *         其他 - 压缩/解压失败
     */
    virtual int write(Buffer::ptr buffer, size_t length) override;

    /**
     * brief: 关闭压缩流
     */
    virtual void close() override;

    /**
     * brief: 输出压缩/解压后的数据
     */
    virtual std::string toString() override;

private:
    /**
     * brief: 初始化压缩流
     * param: type 压缩格式
     *        level 压缩等级(-1: 默认, 0: 不压缩, 1~9 数值越大压缩强度越大)
     *        windows_bits 取值和压缩方式: -(15 ~ 8) : 纯deflate压缩
     *                                       8 ~ 15  : 带zlib头和尾
     *                                         > 15  : 带gzip头和尾
     *        memlevel 运行过程中的内存限制(>=1 && <= 9)
     *        strategy 压缩策略
     * return: 0    - 初始化成功
     *         其他 - 初始化失败
     */
    int init(Type type = DEFLATE, int level = -1, int window_bits = 15,
            int memlevel = 8, Strategy strategy = DEFAULT);

    /**
     * brief: 执行压缩/解压数据
     * param: encode true表示压缩, false表示解压
     *        v 待执行操作的数据
     * return: 0    - 成功
     *         其他 - 失败
     */
    int execute(bool encode, const std::vector<iovec>& v);

private:
    /**
     * brief: zlib压缩读无效
     */
    virtual int read(void* buffer, size_t length) override {}
    /**
     * brief: zlib压缩读无效
     */
    virtual int read(Buffer::ptr buffer, size_t length) override {}

public:
    static ZlibStream::ptr create(bool encode, Type type = DEFLATE, int level = -1,
                               uint32_t buff_size = 4096, int window_bits = 15,
                               int memlevel = 8, Strategy strategy = DEFAULT);

private:
    // 压缩块大小
    uint32_t m_buffSize;
    // 0 表示解压, 1 表示压缩
    bool m_encode;
    // 压缩流
    z_stream m_zstream;
    // 压缩缓存
    std::vector<iovec> m_buffers;
};

}

#endif /*__BIFANG_ZLIB_STREAM_H*/
