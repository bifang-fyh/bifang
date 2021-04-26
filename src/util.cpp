#include "util.h"
#include "log.h"
#include "fiber.h"
#include "config.h"

namespace bifang
{

SystemLogger();

namespace terminal
{

void set_color(Color c)
{
    printf("\033[01;%dm", c);
}

void reset_color()
{
    printf("\033[0m");
}

}

void printError(const std::string& s)
{
    log_error << s;
}

uint64_t getCurrentMS()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    
    return t.tv_sec * 1000ul + t.tv_usec / 1000;
}

uint64_t getCurrentUS()
{
    struct timeval t;
    gettimeofday(&t, NULL);

    return t.tv_sec * 1000000ul + t.tv_usec;
}

std::string time_to_string(time_t ts, const std::string& format, bool is_gmt)
{
    struct tm t;
    if (is_gmt)
        gmtime_r(&ts, &t);
    else
        localtime_r(&ts, &t);
    char buf[64] = {0};
    strftime(buf, sizeof(buf), format.c_str(), &t);
    return buf;
}

time_t string_to_time(const std::string& str, const std::string& format)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    if (!strptime(str.c_str(), format.c_str(), &t))
        return 0;

    return mktime(&t);
}

std::string time_difference(time_t ts)
{
    std::stringstream ss;
    bool flag = false;

    if (ts >= 86400) // days
    {
        ss << (ts / 86400) << "d ";
        ts = ts % (86400);
        flag = true;
    }

    if (ts >= 3600) // hours
    {
        ss << (ts / 3600) << "h ";
        ts = ts % 3600;
        flag = true;
    }
    else if (flag)
    {
        ss << "0h ";
    }

    if (ts >= 60) // minutes
    {
        ss << (ts / 60) << "m ";
        ts = ts % 60;
    }
    else if (flag)
    {
        ss << "0m ";
    }

    ss << ts << "s";
    return ss.str();
}

int Srand()
{
    srand(time(NULL));
    return 0;
}

std::string getHostName()
{
    std::string name;
    name.resize(512);
    gethostname(&name[0], 512);
    return name;
}

// static
static in_addr_t getIPv4Inet()
{
    struct ifaddrs* ifas = nullptr;
    struct ifaddrs* ifa = nullptr;

    in_addr_t localhost = inet_addr("127.0.0.1");
    if (getifaddrs(&ifas))
    {
        log_error << "getifaddrs errno=" << errno << ", errstr=" << strerror(errno);
        return localhost;
    }
    in_addr_t ipv4 = localhost;
    for (ifa = ifas; ifa && ifa->ifa_addr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family != AF_INET)
            continue;
        if (!strncasecmp(ifa->ifa_name, "lo", 2))
            continue;
        ipv4 = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
        if (ipv4 == localhost)
            continue;
    }
    if (ifas != nullptr)
        freeifaddrs(ifas);
    return ipv4;
}

// static
static std::string __getIPv4()
{
    std::string ipv4;
    ipv4.resize(INET_ADDRSTRLEN);
    in_addr_t ia = getIPv4Inet();
    inet_ntop(AF_INET, &ia, &ipv4[0], INET_ADDRSTRLEN);
    return ipv4;
}

std::string getIPv4()
{
    static std::string ip = __getIPv4();
    return ip;
}


namespace CryptUtil
{

static const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64_encode(const std::string &src)
{
    size_t len = src.size();
    std::string result;
    result.resize((src.size() / 3 + 1) * 4);
    const unsigned char* bindata = (unsigned char*)&src[0];
    size_t i, j;
    unsigned char current;

    for (i = 0, j = 0; i < len; i += 3)
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        result[j++] = base64char[(int)current];

        current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
        if (i + 1 >= len)
        {
            result[j++] = base64char[(int)current];
            result[j++] = '=';
            result[j++] = '=';
            break;
        }
        current |= ((unsigned char)(bindata[i+1] >> 4)) & ((unsigned char) 0x0F);
        result[j++] = base64char[(int)current];

        current = ((unsigned char)(bindata[i+1] << 2)) & ((unsigned char)0x3C);
        if (i + 2 >= len)
        {
            result[j++] = base64char[(int)current];
            result[j++] = '=';
            break;
        }
        current |= ((unsigned char)(bindata[i+2] >> 6)) & ((unsigned char) 0x03);
        result[j++] = base64char[(int)current];

        current = ((unsigned char)bindata[i+2]) & ((unsigned char)0x3F) ;
        result[j++] = base64char[(int)current];
    }
    result.resize(j);
    return result;
}

std::string base64_decode(const std::string &src)
{
    std::string result;
    result.resize(src.size() * 3 / 4);
    const char* base64 = &src[0];
    size_t i, j;
    unsigned char k;
    unsigned char temp[4];
    for (i = 0, j = 0; base64[i] != '\0' ; i += 4)
    {
        unsigned char flag = 0x0f;
        memset(temp, 0xFF, sizeof(temp));
        for (k = 0; k < 64; k++)
        {
            if (base64char[k] == base64[i])
            {
                temp[0]= k;
                flag &= ~0x01;
            }
            if (base64char[k] == base64[i+1])
            {
                temp[1]= k;
                flag &= ~0x02;
            }
            if (base64char[k] == base64[i+2])
            {
                temp[2]= k;
                flag &= ~0x04;
            }
            if (base64char[k] == base64[i+3])
            {
                temp[3]= k;
                flag &= ~0x08;
            }

            if (!flag)
                break;
        }

        result[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC))
                    | ((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
        if (base64[i+2] == '=')
            break;

        result[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0))
                    | ((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
        if (base64[i+3] == '=')
            break;

        result[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xC0))
                    | ((unsigned char)(temp[3] & 0x3F));
    }
    result.resize(j);
    return result;
}

// static
static std::string sha1sum(const void* data, size_t len)
{
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, data, len);
    std::string result;
    result.resize(SHA_DIGEST_LENGTH);
    SHA1_Final((unsigned char*)&result[0], &ctx);
    return result;
}

std::string sha1sum(const std::string &data)
{
    return sha1sum(data.c_str(), data.size());
}

#define F(x, y, z)    ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)    ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)    ((x) ^ (y) ^ (z))
#define I(x, y, z)    ((y) ^ ((x) | ~(z)))

#define STEP(f, a, b, c, d, x, t, s) \
    (a) += f((b), (c), (d)) + (x) + (t); \
    (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
    (a) += (b)

#define SET(n)    (*(uint32_t *) &p[n * 4])
#define GET(n)    (*(uint32_t *) &p[n * 4])

static const unsigned char* MD5Body(MD5_CTX* ctx, const unsigned char* data, size_t size)
{
    uint32_t saved_a, saved_b, saved_c, saved_d;
    const unsigned char *p = data;
    uint32_t a = ctx->a;
    uint32_t b = ctx->b;
    uint32_t c = ctx->c;
    uint32_t d = ctx->d;

    do
    {
        saved_a = a;
        saved_b = b;
        saved_c = c;
        saved_d = d;

        /* Round 1 */
        STEP(F, a, b, c, d, SET(0),  0xd76aa478,  7);
        STEP(F, d, a, b, c, SET(1),  0xe8c7b756, 12);
        STEP(F, c, d, a, b, SET(2),  0x242070db, 17);
        STEP(F, b, c, d, a, SET(3),  0xc1bdceee, 22);
        STEP(F, a, b, c, d, SET(4),  0xf57c0faf,  7);
        STEP(F, d, a, b, c, SET(5),  0x4787c62a, 12);
        STEP(F, c, d, a, b, SET(6),  0xa8304613, 17);
        STEP(F, b, c, d, a, SET(7),  0xfd469501, 22);
        STEP(F, a, b, c, d, SET(8),  0x698098d8,  7);
        STEP(F, d, a, b, c, SET(9),  0x8b44f7af, 12);
        STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17);
        STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22);
        STEP(F, a, b, c, d, SET(12), 0x6b901122,  7);
        STEP(F, d, a, b, c, SET(13), 0xfd987193, 12);
        STEP(F, c, d, a, b, SET(14), 0xa679438e, 17);
        STEP(F, b, c, d, a, SET(15), 0x49b40821, 22);

        /* Round 2 */
        STEP(G, a, b, c, d, GET(1),  0xf61e2562,  5);
        STEP(G, d, a, b, c, GET(6),  0xc040b340,  9);
        STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14);
        STEP(G, b, c, d, a, GET(0),  0xe9b6c7aa, 20);
        STEP(G, a, b, c, d, GET(5),  0xd62f105d,  5);
        STEP(G, d, a, b, c, GET(10), 0x02441453,  9);
        STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14);
        STEP(G, b, c, d, a, GET(4),  0xe7d3fbc8, 20);
        STEP(G, a, b, c, d, GET(9),  0x21e1cde6,  5);
        STEP(G, d, a, b, c, GET(14), 0xc33707d6,  9);
        STEP(G, c, d, a, b, GET(3),  0xf4d50d87, 14);
        STEP(G, b, c, d, a, GET(8),  0x455a14ed, 20);
        STEP(G, a, b, c, d, GET(13), 0xa9e3e905,  5);
        STEP(G, d, a, b, c, GET(2),  0xfcefa3f8,  9);
        STEP(G, c, d, a, b, GET(7),  0x676f02d9, 14);
        STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20);

        /* Round 3 */
        STEP(H, a, b, c, d, GET(5),  0xfffa3942,  4);
        STEP(H, d, a, b, c, GET(8),  0x8771f681, 11);
        STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16);
        STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23);
        STEP(H, a, b, c, d, GET(1),  0xa4beea44,  4);
        STEP(H, d, a, b, c, GET(4),  0x4bdecfa9, 11);
        STEP(H, c, d, a, b, GET(7),  0xf6bb4b60, 16);
        STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23);
        STEP(H, a, b, c, d, GET(13), 0x289b7ec6,  4);
        STEP(H, d, a, b, c, GET(0),  0xeaa127fa, 11);
        STEP(H, c, d, a, b, GET(3),  0xd4ef3085, 16);
        STEP(H, b, c, d, a, GET(6),  0x04881d05, 23);
        STEP(H, a, b, c, d, GET(9),  0xd9d4d039,  4);
        STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11);
        STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16);
        STEP(H, b, c, d, a, GET(2),  0xc4ac5665, 23);

        /* Round 4 */
        STEP(I, a, b, c, d, GET(0),  0xf4292244,  6);
        STEP(I, d, a, b, c, GET(7),  0x432aff97, 10);
        STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15);
        STEP(I, b, c, d, a, GET(5),  0xfc93a039, 21);
        STEP(I, a, b, c, d, GET(12), 0x655b59c3,  6);
        STEP(I, d, a, b, c, GET(3),  0x8f0ccc92, 10);
        STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15);
        STEP(I, b, c, d, a, GET(1),  0x85845dd1, 21);
        STEP(I, a, b, c, d, GET(8),  0x6fa87e4f,  6);
        STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10);
        STEP(I, c, d, a, b, GET(6),  0xa3014314, 15);
        STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21);
        STEP(I, a, b, c, d, GET(4),  0xf7537e82,  6);
        STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10);
        STEP(I, c, d, a, b, GET(2),  0x2ad7d2bb, 15);
        STEP(I, b, c, d, a, GET(9),  0xeb86d391, 21);

        a += saved_a;
        b += saved_b;
        c += saved_c;
        d += saved_d;

        p += 64;
    }
    while (size -= 64);

    ctx->a = a;
    ctx->b = b;
    ctx->c = c;
    ctx->d = d;

    return p;
}

void MD5Init(MD5_CTX *ctx)
{
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;
    ctx->bytes = 0;
}

void MD5Update(MD5_CTX *ctx, const void *data, size_t size)
{
    size_t used = (size_t)(ctx->bytes & 0x3f);
    ctx->bytes += size;

    if (used)
    {
        size_t free = 64 - used;

        if (size < free)
        {
            memcpy(&ctx->buffer[used], data, size);
            return;
        }

        memcpy(&ctx->buffer[used], data, free);
        data = (unsigned char*)data + free;
        size -= free;
        MD5Body(ctx, ctx->buffer, 64);
    }

    if (size >= 64)
    {
        data = MD5Body(ctx, (unsigned char*)data, size & ~(size_t)0x3f);
        size &= 0x3f;
    }

    memcpy(ctx->buffer, data, size);
}

void MD5Final(MD5_CTX* ctx, unsigned char result[16])
{  
    size_t used = (size_t)(ctx->bytes & 0x3f);
    ctx->buffer[used++] = 0x80;
    size_t free = 64 - used;

    if (free < 8)
    {
        memset(&ctx->buffer[used], 0x00, free);
        MD5Body(ctx, ctx->buffer, 64);
        used = 0;
        free = 64;
    }

    memset(&ctx->buffer[used], 0x00, free - 8);

    ctx->bytes <<= 3;
    ctx->buffer[56] = (unsigned char)(ctx->bytes);
    ctx->buffer[57] = (unsigned char)(ctx->bytes >> 8);
    ctx->buffer[58] = (unsigned char)(ctx->bytes >> 16);
    ctx->buffer[59] = (unsigned char)(ctx->bytes >> 24);
    ctx->buffer[60] = (unsigned char)(ctx->bytes >> 32);
    ctx->buffer[61] = (unsigned char)(ctx->bytes >> 40);
    ctx->buffer[62] = (unsigned char)(ctx->bytes >> 48);
    ctx->buffer[63] = (unsigned char)(ctx->bytes >> 56);

    MD5Body(ctx, ctx->buffer, 64);

    result[0]  = (unsigned char)(ctx->a);
    result[1]  = (unsigned char)(ctx->a >> 8);
    result[2]  = (unsigned char)(ctx->a >> 16);
    result[3]  = (unsigned char)(ctx->a >> 24);
    result[4]  = (unsigned char)(ctx->b);
    result[5]  = (unsigned char)(ctx->b >> 8);
    result[6]  = (unsigned char)(ctx->b >> 16);
    result[7]  = (unsigned char)(ctx->b >> 24);
    result[8]  = (unsigned char)(ctx->c);
    result[9]  = (unsigned char)(ctx->c >> 8);
    result[10] = (unsigned char)(ctx->c >> 16);
    result[11] = (unsigned char)(ctx->c >> 24);
    result[12] = (unsigned char)(ctx->d);
    result[13] = (unsigned char)(ctx->d >> 8);
    result[14] = (unsigned char)(ctx->d >> 16);
    result[15] = (unsigned char)(ctx->d >> 24);

    memset(ctx, 0x00, sizeof(*ctx));
}

std::string MD5Create16(const std::string& src, bool lower)
{
    std::string str = MD5Create32(src, lower);
    return str.substr(8, 16);
}

std::string MD5Create32(const std::string& src, bool lower)
{
    unsigned char digest[16] = {0};
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)&src[0], src.size());
    MD5Final(&md5, digest);

    static const char table[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string str;
    str.resize(32);
    for (size_t i = 0; i < 16; i++)
    {
        str[i * 2] = table[digest[i] / 16];
        str[i * 2 + 1] = table[digest[i] % 16];
    }

    if (!lower)
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

    return str;
}

// static
static void crypt_to64(std::string& str, uint32_t v, size_t n)
{
    static const char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    while (n--)
    {
        str.append(1, itoa64[v & 0x3f]);
        v >>= 6;
    }
}

std::string crypt_apr1(const std::string& key, const std::string& salt)
{
    unsigned char __final[16];
    unsigned char* __key = (unsigned char*)&key[0];
    size_t keylen = key.size();

    unsigned char* __salt = (unsigned char*)&salt[0];
    __salt += sizeof("$apr1$") - 1;
    unsigned char* last = __salt + 8;
    unsigned char* p;
    for (p = __salt; *p && *p != '$' && p < last; p++);
    size_t saltlen = p - __salt;

    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, __key, keylen);
    MD5Update(&md5, (unsigned char*)"$apr1$", sizeof("$apr1$") - 1);
    MD5Update(&md5, __salt, saltlen);

    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, __key, keylen);
    MD5Update(&ctx, __salt, saltlen);
    MD5Update(&ctx, __key, keylen);
    MD5Final(&ctx, __final);

    for (int n = keylen; n > 0; n -= 16)
        MD5Update(&md5, __final, n > 16 ? 16 : n);

    memset(__final, 0x00, sizeof(__final));

    for (size_t i = keylen; i; i >>= 1)
    {
        if (i & 1)
            MD5Update(&md5, __final, 1);
        else
            MD5Update(&md5, __key, 1);
    }

    MD5Final(&md5, __final);

    for (size_t i = 0; i < 1000; i++)
    {
        MD5Init(&ctx);

        if (i & 1)
            MD5Update(&ctx, __key, keylen);
        else
            MD5Update(&ctx, __final, 16);

        if (i % 3)
            MD5Update(&ctx, __salt, saltlen);

        if (i % 7)
            MD5Update(&ctx, __key, keylen);

        if (i & 1)
            MD5Update(&ctx, __final, 16);
        else
            MD5Update(&ctx, __key, keylen);

        MD5Final(&ctx, __final);
    }

    /* output */
    std::string ret = "$apr1$";
    ret.append((char*)__salt, 0, saltlen);
    ret.append(1, '$');

    crypt_to64(ret, (__final[0]<<16) | (__final[6]<<8)  | __final[12], 4);
    crypt_to64(ret, (__final[1]<<16) | (__final[7]<<8)  | __final[13], 4);
    crypt_to64(ret, (__final[2]<<16) | (__final[8]<<8)  | __final[14], 4);
    crypt_to64(ret, (__final[3]<<16) | (__final[9]<<8)  | __final[15], 4);
    crypt_to64(ret, (__final[4]<<16) | (__final[10]<<8) | __final[5],  4);
    crypt_to64(ret, __final[11], 2);

    return ret;
}

}


namespace FileUtil
{

bool __lstat(const std::string& filename, struct stat* st)
{
    struct stat lst;
    int ret = lstat(filename.c_str(), &lst);
    if (st)
        *st = lst;

    return !ret;
}

bool __mkdir(const std::string& dirname, mode_t mode)
{
    struct stat lst;
    if (__lstat(dirname, &lst))
    {
        if (S_ISDIR(lst.st_mode))
            return true;
        else
            return false;
    }

    return !mkdir(dirname.c_str(), mode);
}

bool Mkdir(const std::string& dirname, mode_t mode)
{
    struct stat lst;
    if (__lstat(dirname, &lst))
    {
        if (S_ISDIR(lst.st_mode))
            return true;
        else
            return false;
    }

    std::string path = dirname;
    size_t last = 0;
    size_t pos = 0;
    
    while (1)
    {
        last = pos + 1;
        pos = path.find('/', last);
        if (pos == std::string::npos)
            break;

        if (!__mkdir(path.substr(0, pos), mode))
            return false;
    }

    if (last != path.size() && !__mkdir(path, mode))
        return false;

    return true;
}

bool CheckPidfile(const std::string& filename, pid_t* pid_return)
{
    if (!__lstat(filename))
    {
        return false;
    }
    std::ifstream ifs(filename);
    std::string line;
    if (!ifs || !std::getline(ifs, line))
    {
        return false;
    }
    if (line.empty())
    {
        return false;
    }
    pid_t pid = atoi(line.c_str());
    if (pid <= 1)
    {
        return false;
    }
    //std::cout << pid << std::endl;
    if (kill(pid, 0))
    {
        return false;
    }
    if (pid_return)
        *pid_return = pid;
    return true;
}

bool Rm(const std::string& path)
{
    struct stat lst;
    if (!__lstat(path, &lst)) // 文件原本就不存在
        return true;
    if (!S_ISDIR(lst.st_mode)) // 非文件夹
        return Unlink(path);

    DIR* dir = opendir(path.c_str());
    if (!dir)
    {
        log_warn << "open dir(" << path << ") fail";
        return false;
    }

    struct dirent* dp = nullptr;
    while (dp = readdir(dir))
    {
        if (dp->d_name[0] == '.')
            continue;

        std::string path_sub;
        if (path[path.size() - 1] == '/')
            path_sub = path + dp->d_name;
        else
            path_sub = path + "/" + dp->d_name;

        if (!Rm(path_sub))
            return false;
    }
    closedir(dir);
    if (rmdir(path.c_str()))
    {
        log_warn << "rm dir(" << path << ") fail";
        return false;
    }

    return true;
}

bool Mv(const std::string& from, const std::string& to)
{
    if (!Rm(to))
    {
        log_warn << "rm destination path(" << to << ") fail";
        return false;
    }

    return !rename(from.c_str(), to.c_str());
}

bool Cp(const std::string& from, const std::string& to)
{
    static const size_t size = 10 * 1024;
    struct stat lst;
    if (!__lstat(from, &lst)) // 源文件或目录不存在
    {
        log_warn << "source path(" << from << ") not exist";
        return false;
    }

    if (!S_ISDIR(lst.st_mode)) // 非文件夹, 复制文件
    {
        std::ifstream ifs(from);
        std::ofstream ofs(to, std::ios::trunc);
        if (!ifs)
        {
            log_warn << "open source file(" << from << ") fail";
            return false;
        }
        if (!ofs)
        {
            log_warn << "open destination file(" << to << ") fail";
            return false;
        }

        char* data = new char[size];
        while (!ifs.eof())
        {
            ifs.read(data, size);
            ofs.write(data, ifs.gcount());
        }
        delete data;
        return true;
    }

    DIR* dir = opendir(from.c_str());
    if (!dir)
    {
        log_warn << "open dir(" << from << ") fail";
        return false;
    }
    if (!__mkdir(to))
    {
        log_warn << "mkdir dir(" << to << ") fail";
        return false;
    }
    struct dirent* dp = nullptr;
    while (dp = readdir(dir))
    {
        if (dp->d_name[0] == '.')
            continue;

        std::string from_sub;
        if (from[from.size() - 1] == '/')
            from_sub = from + dp->d_name;
        else
            from_sub = from + "/" + dp->d_name;

        std::string to_sub;
        if (to[to.size() - 1] != '/')
            to_sub = to + "/" + dp->d_name;
        else
            to_sub = to + dp->d_name;

        if (!Cp(from_sub, to_sub))
            return false;
    }
    closedir(dir);

    return true;
}

bool Realpath(const std::string& path, std::string& rpath)
{
    if (!__lstat(path.c_str()))
        return false;

    char* ptr = realpath(path.c_str(), NULL);
    if (ptr == NULL)
        return false;
    std::string(ptr).swap(rpath);
    free(ptr);

    return true;
}

bool Symlink(const std::string& from, const std::string& to)
{
    if (!Rm(to))
    {
        log_warn << "rm destination path(" << to << ") fail";
        return false;
    }

    return !symlink(from.c_str(), to.c_str());
}

bool Unlink(const std::string& filename)
{
    if (!__lstat(filename)) // 文件原本就不存在
        return true;

    return !unlink(filename.c_str());
}

std::string DirName(const std::string& filename)
{
    if (filename.empty())
        return ".";

    auto pos = filename.rfind('/');

    if (pos == 0)
        return "/";
    else if (pos == std::string::npos)
        return ".";
    else
        return filename.substr(0, pos);
}

std::string Basename(const std::string& filename)
{
    if (filename.empty())
        return filename;

    auto pos = filename.rfind('/');
    if (pos == std::string::npos)
        return filename;
    else
        return filename.substr(pos + 1);
}

std::string PureDirname(const std::string& filename)
{
    std::string name = filename;
    while (name[name.size() - 1] == '/') // 将目录名末尾的/消除
        name = name.substr(0, name.size() - 1);

    while (name.find("./") == 0) // 将开头的./去掉
        name = name.substr(2, name.size());

    return name;
}

bool GetDirInformation(const std::string& dirname, std::vector<FileInfo>& info)
{
    if (access(dirname.c_str(), 0) != 0)
    {
        log_warn << "destination dir(" << dirname << ") fail";
        return false;
    }

    DIR* dir = opendir(dirname.c_str());
    if (dir == NULL)
    {
        log_warn << "open dir(" << dirname << ") fail";
        return false;
    }

    struct dirent* dp = NULL;
    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_name[0] == '.')
            continue;

        std::string sub_name = dirname + dp->d_name;

        struct stat lst;
        lstat(sub_name.c_str(), &lst);

        FileInfo fi;
        fi.name = dp->d_name;
        if (S_ISDIR(lst.st_mode))
            fi.name += "/";
        fi.mtime = lst.st_mtime;
        fi.ctime = lst.st_ctime;
        fi.size = lst.st_size;

        info.push_back(fi);
    }
    closedir(dir);
    /**
     * details: 排序, 目录在前, 文件在后, 同级则按名称的字典序排列
     */
    sort(info.begin(), info.end(), [](const FileInfo& f1, const FileInfo& f2)
        {
            if ((f1.name[f1.name.size() - 1] == '/' && f2.name[f2.name.size() - 1] == '/')
             || (f1.name[f1.name.size() - 1] != '/' && f2.name[f2.name.size() - 1] != '/'))
                return f1.name < f2.name;
            else if (f1.name[f1.name.size() - 1] == '/')
                return true;
            else
                return false;
        });
    return true;
}

bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
         std::ios_base::openmode mode)
{
    ofs.open(filename, mode);
    if (!ofs.is_open())
    {
        std::string dir = DirName(filename);
        Mkdir(dir);
        ofs.open(filename, mode);
    }
    return ofs.is_open();
}

bool OpenForRead(std::ifstream& ifs, const std::string& filename,
         std::ios_base::openmode mode)
{
    ifs.open(filename, mode);

    return ifs.is_open();
}

static bool IsSubfix(const std::string& filename, const std::string& subfix)
{
    if (filename.size() < subfix.size())
        return false;
    return !strcasecmp(filename.substr(filename.length() - subfix.size()).c_str(), subfix.c_str());
}

void ListAllFile(std::vector<std::string>& files, const std::string& dirname,
         const std::string& subfix)
{
    if (access(dirname.c_str(), 0) != 0)
    {
        log_warn << "destination dir(" << dirname << ") fail";
        return;
    }

    DIR* dir = opendir(dirname.c_str());
    if (dir == NULL)
    {
        log_warn << "open dir(" << dirname << ") fail";
        return;
    }

    struct dirent* dp = NULL;
    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_type == DT_DIR)
        {
            if (dp->d_name[0] == '.')
                continue;

            ListAllFile(files, dirname + "/" + dp->d_name, subfix);
        }
        else if (dp->d_type == DT_REG)
        {
            std::string filename(dp->d_name);
            if (!subfix.empty() && !IsSubfix(filename, subfix))
                continue;
            files.push_back(dirname + "/" + filename);
        }
    }
    closedir(dir);
}

}

namespace StringUtil
{

std::string urlEncode(const std::string& str, bool is_path)
{
    static uint32_t uri[] =
    {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x80000029, /* 1000 0000 0000 0000  0000 0000 0010 1001 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    static uint32_t uri_component[] =
    {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0xfc009fff, /* 1111 1100 0000 0000  1001 1111 1111 1111 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x78000001, /* 0111 1000 0000 0000  0000 0000 0000 0001 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xb8000001, /* 1011 1000 0000 0000  0000 0000 0000 0001 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    static const char* hex = "0123456789ABCDEF";

    uint32_t* escape = is_path ? uri : uri_component;
    std::string ret;
    for (auto c : str)
    {
        unsigned char ch = (unsigned char)c;
        if (escape[ch >> 5] & (1U << (ch & 0x1F)))
        {
            ret.append(1, '%');
            ret.append(1, hex[ch >> 4]);
            ret.append(1, hex[ch & 0xF]);
        }
        else
            ret.append(1, ch);
    }
    return ret;
}

std::string urlDecode(const std::string& str, bool is_path)
{
    static const char xdigit_chars[256] =
    {
        /* 0 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
        /* 64 */
        0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 128 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 192 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for (const char* c = str.c_str(); c < end; c++)
    {
        if (*c == '+' && !is_path)
        {
            if (!ss)
            {
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, ' ');
        }
        else if (*c == '%' && (c + 2) < end && isxdigit(*(c + 1))
            && isxdigit(*(c + 2)))
        {
            if (!ss)
            {
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
            c += 2;
        }
        else if (ss)
            ss->append(1, *c);
    }

    if (!ss)
        return str;
    else
    {
        std::string ret = *ss;
        delete ss;
        return ret;
    }
}

std::string escapeHtml(const std::string& str)
{
    std::string ret;
    for (char ch : str)
    {
        switch (ch)
        {
            case '<':
                ret.append("&lt;");
                break;

            case '>':
                ret.append("&gt;");
                break;

            case '&':
                ret.append("&amp;");
                break;

            case '"':
                ret.append("&quot;");
                break;

            default:
                ret.append(1, ch);
                break;
        }
    }

    return ret;
}

/**
 * brief: 计算当前utf8字符所占字节数
 * param: ch 起始字符
 * details: UTF coding(中文可能是3字节, 也可能是4字节)
 *              1字节 0xxxxxxx
 *              2字节 110xxxxx 10xxxxxx
 *              3字节 1110xxxx 10xxxxxx 10xxxxxx
 *              4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *              5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *              6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
static int count_UTF8_byte(char ch)
{
#define XX(num) \
    if (ch & num) \
        count++; \
    else \
        goto good

    int count = 0;
    XX(0x80);
    XX(0x40);
    XX(0x20);
    XX(0x10);
    XX(0x08);
    XX(0x04);

good:
    return count ? count : 1;
#undef XX
}

int countCompensation(const std::string& str, int offset)
{
    int count = 0;
    for (size_t i = 0; i < str.size();)
    {
        int n = count_UTF8_byte(str[i]);
        if (n >= 3)
            count += n - offset;
        i += n;
    }
    return count;
}

std::vector<int> countOriginalLength(const std::string& str, int length, int offset)
{
    int count = 0;
    for (size_t i = 0; i < str.size();)
    {
        int n = count_UTF8_byte(str[i]);
        length -= (n >= 3 ? offset : 1);
        if (length < 0)
            break;
        count += n;
        i += n;
        if (length == 0)
            break;
    }
    return {count, (length ? length + offset : 0)};
}

std::string findSubPath(const std::string& path, const std::string& root)
{
    size_t pos = path.find(root);
    if (pos == std::string::npos)
        return "";
    return path.substr(pos + root.size());
}

std::string trim(const std::string& str, const std::string& delimit)
{
    auto begin = str.find_first_not_of(delimit);
    if (begin == std::string::npos)
        return "";

    auto end = str.find_last_not_of(delimit);
    return str.substr(begin, end - begin + 1);
}

std::string trimLeft(const std::string& str, const std::string& delimit)
{
    auto begin = str.find_first_not_of(delimit);
    if (begin == std::string::npos)
        return "";

    return str.substr(begin);
}

std::string trimRight(const std::string& str, const std::string& delimit)
{
    auto end = str.find_last_not_of(delimit);
    if (end == std::string::npos)
        return "";

    return str.substr(0, end);
}

std::vector<std::string> split(const std::string &str, char delim, size_t max)
{
    std::vector<std::string> result;
    if (str.empty())
        return result;

    size_t last = 0;
    size_t pos = str.find(delim);
    while (pos != std::string::npos)
    {
        result.push_back(str.substr(last, pos - last));
        last = pos + 1;
        if (--max == 1)
            break;
        pos = str.find(delim, last);
    }
    result.push_back(str.substr(last));
    return result;
}

std::vector<std::string> split(const std::string &str,
                             const std::string& delims, size_t max)
{
    std::vector<std::string> result;
    if (str.empty())
        return result;

    size_t last = 0;
    size_t pos = str.find_first_of(delims);
    while (pos != std::string::npos)
    {
        result.push_back(str.substr(last, pos - last));
        last = pos + 1;
        if (--max == 1)
            break;
        pos = str.find_first_of(delims, last);
    }
    result.push_back(str.substr(last));
    return result;
}

std::string randomString(size_t len, const std::string& chars)
{
    if (len == 0 || chars.empty())
        return "";

    static int s = Srand();
    std::string str;
    str.resize(len);
    size_t count = chars.size();
    for (size_t i = 0; i < len; i++)
        str[i] = chars[rand() % count];

    return str;
}

std::string toHexString(const std::string& str, bool is_upper)
{
    std::stringstream ss;
    ss << std::hex;
    if (is_upper)
        ss << std::uppercase;

    for (auto& ch : str)
        ss << std::setw(2) << std::setfill('0') << (unsigned int)(unsigned char)ch;

    return ss.str();
}

std::string toBinString(const std::string& str)
{
    if (str.size() % 2 != 0)
        return "";

    std::string res;
    for (size_t i = 0; i < str.size() - 1; i += 2)
    {
        unsigned char byte;
        switch (str[i])
        {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                byte = (str[i] - 'a' + 10) << 4;
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                byte = (str[i] - 'A' + 10) << 4;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                byte = (str[i] - '0') << 4;
                break;
            default:
                return "";
        }
        switch (str[i + 1])
        {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                byte |= (str[i + 1] - 'a' + 10);
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                byte |= (str[i + 1] - 'A' + 10);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                byte |= (str[i + 1] - '0');
                break;
            default:
                return "";
        }
        res.append(1, (char)byte);
    }
    return res;
}

}


namespace JsonUtil
{

std::string getString(const Json::Value& root, const std::string& name,
                const std::string& default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isString())
        return v.asString();

    return default_value;
}

double getDouble(const Json::Value& root, const std::string& name,
           double default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isDouble())
        return v.asDouble();
    else if (v.isString())
        return atof(v.asString().c_str());

    return default_value;
}

int32_t getInt32(const Json::Value& root, const std::string& name,
            int32_t default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isInt())
        return v.asInt();
    else if (v.isString())
        return atoi(v.asString().c_str());

    return default_value;
}

uint32_t getUint32(const Json::Value& root, const std::string& name,
             uint32_t default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isUInt())
        return v.asUInt();
    else if (v.isString())
        return atoi(v.asString().c_str());

    return default_value;
}

int64_t getInt64(const Json::Value& root, const std::string& name,
            int64_t default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isInt64())
        return v.asInt64();
    else if (v.isString())
        return atoll(v.asString().c_str());

    return default_value;
}

uint64_t getUint64(const Json::Value& root, const std::string& name,
             uint64_t default_value)
{
    if (!root.isMember(name))
        return default_value;

    auto& v = root[name];
    if (v.isUInt64())
        return v.asUInt64();
    else if (v.isString())
        return atoll(v.asString().c_str());

    return default_value;
}

Json::Value fromString(const std::string& v)
{
    SEQUENCE(root, v, "json string(transfer) is illegal!");
    return root;
}

std::string toString(const Json::Value& root)
{
    RESEQUENCE(root);
}

}

}
