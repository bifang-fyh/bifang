#ifndef __BIFANG_SOCKET_STREAM_H
#define __BIFANG_SOCKET_STREAM_H


#include "stream.h"
#include "socket.h"


namespace bifang
{

/**
 * brief: Socket流
 */
class SocketStream : public Stream
{
public:
    typedef std::shared_ptr<SocketStream> ptr;

    /**
     * brief: 构造函数
     * param: sock Socket类
     */
    SocketStream(Socket::ptr sock);

    /**
     * brief: 析构函数
     */
    ~SocketStream();

    /**
     * brief: 读取数据
     * param: buffer 待接收数据的内存
     *        length 待接收数据的内存长度
     * return: >0 返回实际接收到的数据长度
     *         =0 socket被远端关闭
     *         <0 socket错误
     */
    virtual int read(void* buffer, size_t length) override;

    /**
     * brief: 读取数据
     * param: buffer 接收数据的Buffer
     *        length 待接收数据的内存长度
     * return: >0 返回实际接收到的数据长度
     *         =0 socket被远端关闭
     *         <0 socket错误
     */
    virtual int read(Buffer::ptr buffer, size_t length) override;

    /**
     * brief: 写入数据
     * param: buffer 待发送数据的内存
     *        length 待发送数据的内存长度
     * return: >0 返回实际接收到的数据长度
     *         =0 socket被远端关闭
     *         <0 socket错误
     */
    virtual int write(const void* buffer, size_t length) override;

    /**
     * brief: 写入数据
     * param: buffer 待发送数据的Buffer
     *        length 待发送数据的内存长度
     * return: >0 返回实际接收到的数据长度
     *         =0 socket被远端关闭
     *         <0 socket错误
     */
    virtual int write(Buffer::ptr buffer, size_t length) override;

    /**
     * brief: 关闭socket
     */
    virtual void close() override;

    Socket::ptr getSocket() const { return m_socket; }
    bool isConnected() const { return m_socket && m_socket->isConnected(); }

    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();

    virtual std::string toString() override;

protected:
    // Socket类
    Socket::ptr m_socket;
};

}

#endif /*__BIFANG_SOCKET_STREAM_H*/
