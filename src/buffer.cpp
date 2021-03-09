#include "buffer.h"
#include "log.h"
#include "util.h"


namespace bifang
{

SystemLogger();

// ZigZag编码
static inline uint32_t encodeZigZag32(const int32_t& n)
{
    return (n << 1) ^ (n >> 31);
}

static inline uint64_t encodeZigZag64(const int64_t& n)
{
    return (n << 1) ^ (n >> 63);
}

static inline int32_t decodeZigZag32(const uint32_t& n)
{
    return (n >> 1) ^ -(n & 1);
}

static inline int64_t decodeZigZag64(const uint64_t& n)
{
    return (n >> 1) ^ -(n & 1);
}


Buffer::Buffer(size_t base_size)
    :m_baseSize(base_size)
    ,m_position(0)
    ,m_capacity(base_size)
    ,m_size(0)
    ,m_root(new Node(base_size))
    ,m_cur(m_root)
{
}

Buffer::~Buffer()
{
    Node* tmp = m_root;
    while (tmp)
    {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

void Buffer::setPosition(size_t v, bool movement)
{
    if (v > m_capacity)
        throw std::out_of_range("setPosition out of range");
    m_position = v;
    if (m_position > m_size)
        m_size = m_position;

    if (movement)
    {
        m_cur = m_root;
        while (v >= m_cur->size)
        {
            v -= m_cur->size;
            if (m_cur == nullptr)
                throw std::out_of_range("m_cur out of range");
            m_cur = m_cur->next;
        }
    }
}

void Buffer::init()
{
    m_position = 0;
    m_size = 0;
    m_cur = m_root;
}

void Buffer::clear()
{
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node* tmp = m_root->next;
    while (tmp)
    {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr;
}

void Buffer::write(const void* buf, size_t size)
{
    if (size == 0)
        return;
    addCapacity(size);

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, size);
            if (m_cur->size == (npos + size))
                m_cur = m_cur->next;
            m_position += size;
            break;
        }
        else
        {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }

    if (m_position > m_size)
        m_size = m_position;
}

void Buffer::read(void* buf, size_t size)
{
    if (size > getReadSize())
        throw std::out_of_range("not enough len");

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, m_cur->ptr + npos, size);
            if (m_cur->size == (npos + size))
                m_cur = m_cur->next;
            m_position += size;
            break;
        }
        else
        {
            memcpy((char*)buf + bpos, m_cur->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
}

void Buffer::read(void* buf, size_t size, size_t position) const
{
    if (size > (m_size - position))
        throw std::out_of_range("read not enough len");

    size_t npos = position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    Node* cur = m_cur;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, cur->ptr + npos, size);
            break;
        }
        else
        {
            memcpy((char*)buf + bpos, cur->ptr + npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

void Buffer::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeInt(v);
}

void Buffer::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeInt(v);
}

void Buffer::writeVarint32(int32_t value)
{
    writeVaruint32(encodeZigZag32(value));
}

void Buffer::writeVaruint32(uint32_t value)
{
    uint8_t tmp[5];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

void Buffer::writeVarint64(int64_t value)
{
    writeVaruint64(encodeZigZag64(value));
}

void Buffer::writeVaruint64(uint64_t value)
{
    uint8_t tmp[10];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

void Buffer::writeString(const std::string& value)
{
    write(value.c_str(), value.size());
}

float Buffer::readFloat(const float& value)
{
    uint32_t v = readInt((uint32_t)1);
    float ret;
    memcpy(&ret, &v, sizeof(v));
    return ret;
}

double Buffer::readDouble(const double& value)
{
    uint64_t v = readInt((uint64_t)1);
    double ret;
    memcpy(&ret, &v, sizeof(v));
    return ret;
}

int32_t Buffer::readVarint32(const int32_t& value)
{
    return decodeZigZag32(readVaruint32());
}

uint32_t Buffer::readVaruint32(const uint32_t& value)
{
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readInt((uint8_t)1);
        result |= (((uint32_t)(b & 0x7f)) << i);
        if (b < 0x80)
            break;
    }
    return result;
}

int64_t Buffer::readVarint64(const int64_t& value)
{
    return decodeZigZag64(readVaruint64());
}

uint64_t Buffer::readVaruint64(const uint64_t& value)
{
    uint64_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readInt((uint8_t)1);
        result |= (((uint64_t)(b & 0x7f)) << i);
        if (b < 0x80)
            break;
    }
    return result;
}

bool Buffer::writeToFile(const std::string& filename) const
{
    std::ofstream ofs;
    if (!FileUtil::OpenForWrite(ofs, filename, std::ios::trunc | std::ios::binary)) 
    {
        log_error << "writeToFile filename=" << filename
            << " error, errno=" << errno << ", errstr=" << strerror(errno);
        return false;
    }

    size_t read_size = getReadSize();
    size_t pos = m_position;
    Node* cur = m_cur;
    while (read_size > 0)
    {
        size_t diff = pos % m_baseSize;
        size_t len = (read_size > m_baseSize ? m_baseSize : read_size) - diff;
        ofs.write(cur->ptr + diff, len);
        cur = cur->next;
        pos += len;
        read_size -= len;
    }

    return true;
}

bool Buffer::readFromFile(const std::string& filename)
{
    std::ifstream ifs;
    if (!FileUtil::OpenForRead(ifs, filename, std::ios::binary))
    {
        log_error << "ReadFromFile filename=" << filename
            << " error, errno=" << errno << ", errstr=" << strerror(errno);
        return false;
    }

    char* data = new char[m_baseSize];
    while (!ifs.eof())
    {
        ifs.read(data, m_baseSize);
        write(data, ifs.gcount());
    }
    delete data;
    return true;
}

std::string Buffer::toString() const
{
    std::string str;
    str.resize(getReadSize());
    if (str.empty())
        return str;
    read(&str[0], str.size(), m_position);
    return str;
}

std::string Buffer::toHexString() const
{
    std::string str = toString();
    std::stringstream ss;

    for (size_t i = 0; i < str.size(); i++)
    {
        if (i % 32 == 0 && i > 0)
            ss << std::endl;
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " ";
    }

    return ss.str();
}

uint64_t Buffer::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const
{
    len = len > getReadSize() ? getReadSize() : len;
    if (len == 0)
        return 0;

    uint64_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;

    while (len > 0)
    {
        if (ncap >= len)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t Buffer::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len)
{
    if (len == 0)
        return 0;
    addCapacity(len);
    uint64_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;
    while (len > 0)
    {
        if(ncap >= len)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

// private
void Buffer::addCapacity(size_t size)
{
    if (size == 0)
        return;
    size_t old_cap = getCapacity();
    if (old_cap >= size) 
        return;

    size -= old_cap;
    size_t count = size / m_baseSize + (size % m_baseSize != 0);
    Node* tmp = m_cur == nullptr ? m_root : m_cur;
    while (tmp->next)
        tmp = tmp->next;

    Node* first = nullptr;
    for (size_t i = 0; i < count; i++)
    {
        tmp->next = new Node(m_baseSize);
        if (first == nullptr) 
            first = tmp->next;
        tmp = tmp->next;
        m_capacity += m_baseSize;
    }

    if (old_cap == 0)
        m_cur = first;
}


}
