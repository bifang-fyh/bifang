/**
 * brief: 流文件基类
 */
#ifndef __BIFANG_STREAM_H
#define __BIFANG_STREAM_H

#include <memory>

#include "buffer.h"

namespace bifang
{

/**
 * brief: 自定义流结构
 */
class Stream
{
public:
    typedef std::shared_ptr<Stream> ptr;

    virtual ~Stream() {}

    /**
     * brief: 读数据
     * param: buffer 接收数据的内存
     *        length 接收数据的内存大小
     * return: >0 返回接收到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int read(void* buffer, size_t length) = 0;
    /**
     * brief: 读数据
     * param: buffer 接收数据的Buffer
     *        length 接收数据的Buffer大小
     * return: >0 返回接收到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int read(Buffer::ptr buffer, size_t length) = 0;
    /**
     * brief: 读固定长度的数据
     * param: buffer 接收数据的内存
     *        length 接收数据的内存大小
     * return: >0 返回接收到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int readFixSize(void* buffer, size_t length);
    /**
     * brief: 读固定长度的数据
     * param: buffer 接收数据的Buffer
     *        length 接收数据的Buffer大小
     * return: >0 返回接收到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int readFixSize(Buffer::ptr buffer, size_t length);

    /**
     * brief: 写数据
     * param: buffer 写数据的内存
     *        length 写入数据的内存大小
     * return: >0 返回写入到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int write(const void* buffer, size_t length) = 0;
    /**
     * brief: 写数据
     * param: buffer 写数据的buffer
     *        length 写入数据的buffer大小
     * return: >0 返回写入到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int write(Buffer::ptr buffer, size_t length) = 0;
    /**
     * brief: 写固定长度的数据
     * param: buffer 写数据的内存
     *        length 写入数据的内存大小
     * return: >0 返回写入到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int writeFixSize(const void* buffer, size_t length);

    /**
     * brief: 写固定长度的数据
     * param: buffer 写数据的buffer
     *        length 写入数据的buffer大小
     * return: >0 返回写入到的数据的实际大小
     *         =0 被关闭
     *         <0 出现流错误
     */
    virtual int writeFixSize(Buffer::ptr buffer, size_t length);

    /**
     * brief: 关闭流
     */
    virtual void close() = 0;

    virtual std::string toString() = 0;
};

}

#endif /*__BIFANG_STREAM_H*/
