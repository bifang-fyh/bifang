/**
 * brief: 大小端转换文件
 */
#ifndef __BIFANG_ENDIAN_CPP_H
#define __BIFANG_ENDIAN_CPP_H

// BYTE_ORDER BIG_ENDIAN LITTLE_ENDIAN

#include <type_traits>
#include <stdint.h>

namespace bifang
{

#define BSWAP_8(x)  ((x) & 0xff)
#define BSWAP_16(x) ((BSWAP_8(x)  <<  8) | BSWAP_8((x)  >>  8))
#define BSWAP_32(x) ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x) ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))

// 8字节类型的字节序转化
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value)
{
    return (T)BSWAP_64((uint64_t)value);
}

// 4字节类型的字节序转化
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value)
{
    return (T)BSWAP_32((uint32_t)value);
}

// 2字节类型的字节序转化
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value)
{
    return (T)BSWAP_16((uint16_t)value);
}

// 1字节类型的字节序转化
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint8_t), T>::type
byteswap(T value)
{
    return (T)BSWAP_8((uint8_t)value);
}


#if BYTE_ORDER == BIG_ENDIAN

// 只在小端机器上执行byteswap, 在大端机器上什么都不做
template<class T>
T littleByteSwap(T t)
{
    return t;
}
// 只在大端机器上执行byteswap, 在小端机器上什么都不做
template<class T>
T bigByteSwap(T t)
{
    return byteswap(t);
}

#else

// 只在小端机器上执行byteswap, 在大端机器上什么都不做
template<class T>
T littleByteSwap(T t)
{
    return byteswap(t);
}
// 只在大端机器上执行byteswap, 在小端机器上什么都不做
template<class T>
T bigByteSwap(T t)
{
    return t;
}

#endif

}

#endif /*__BIFANG_ENDIAN_CPP_H*/
