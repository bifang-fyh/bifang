#include "Bencode.h"
#include "log.h"
#include "Assert.h"

// UTF编码
namespace bifang
{

SystemLogger();

////////// BCodeValue::CZString 
BCodeValue::CZString::CZString(size_t index)
    :m_str(nullptr)
    ,m_index(index)
{
}

BCodeValue::CZString::CZString(const std::string& str)
{
    m_str = new std::string(str);
}

BCodeValue::CZString::CZString(const CZString& other)
{
    if (!other.m_str)
    {
        m_str = nullptr;
        m_index = other.m_index;
    }
    else
        m_str = new std::string(*other.m_str);
}

BCodeValue::CZString::~CZString()
{
    if (m_str)
        delete m_str;
}

BCodeValue::CZString& BCodeValue::CZString::operator=(const CZString& other)
{
    if (m_str)
        delete m_str;
    m_str = nullptr;

    if (other.m_str == nullptr)
        m_index = other.m_index;
    else
        m_str = new std::string(*other.m_str);

    return *this;
}

bool BCodeValue::CZString::operator<(const CZString& other) const
{
    if (!m_str)
        return m_index < other.m_index;

    ASSERT_MSG(m_str && other.m_str, "BCodeValue::CZString::operator<(const CZString& other) couldn't compare different type");

    return *m_str < *other.m_str;
}

bool BCodeValue::CZString::operator==(const CZString& other) const
{
    if (!m_str)
        return m_index == other.m_index;

    ASSERT_MSG(m_str && other.m_str, "BCodeValue::CZString::operator==(const CZString& other) couldn't compare different type");

    return *m_str == *other.m_str;
}
////////// BCodeValue::CZString 


////////// BCodeValue
BCodeValue::BCodeValue(BCodeValue::Type type)
{
    m_type = type;
    switch (m_type)
    {
        case BCODE_INTEGER:
            m_value.m_int = 0;
            break;

        case BCODE_STRING:
            m_value.m_string = new std::string;
            break;

        case BCODE_LIST:
        case BCODE_DICTIONARY:
            m_value.m_map = new DictionaryValues();
            break;

        default:
            ASSERT_MSG(false, "type(" + std::to_string(m_type) + ") is no support");
            break;
    }
}

BCodeValue::BCodeValue(int64_t value)
{
    m_type = BCODE_INTEGER;
    m_value.m_int = value;
}

BCodeValue::BCodeValue(const std::string& value)
{
    m_type = BCODE_STRING;
    m_value.m_string = new std::string(value);
}

BCodeValue::BCodeValue(const BCodeValue& other)
{
    m_type = other.getType();
    switch (m_type)
    {
        case BCODE_INTEGER:
            m_value.m_int = other.m_value.m_int;
            break;

        case BCODE_STRING:
            m_value.m_string = new std::string(*other.m_value.m_string);
            break;
        
        case BCODE_LIST:
        case BCODE_DICTIONARY:
            m_value.m_map = new DictionaryValues(*other.m_value.m_map);
            break;

        default:
            ASSERT_MSG(false, "type(" + std::to_string(m_type) + ") is no support");
            break;
    }
}

BCodeValue::~BCodeValue()
{
    free();
}

size_t BCodeValue::size() const
{
    switch (m_type)
    {
        case BCODE_INTEGER:
        case BCODE_STRING:
            return 0;

        case BCODE_LIST:
        {
            if (m_value.m_map->empty())
                return 0;

            DictionaryValues::const_iterator it = m_value.m_map->end();
            --it;
            return (*it).first.index() + 1;
        }

        case BCODE_DICTIONARY:
        {
            return m_value.m_map->size();
        }

        default:
            return 0;
    }
}

bool BCodeValue::empty() const
{
    if (isList() || isDictionary())
        return size() == 0;
    return false;
}

void BCodeValue::setType(BCodeValue::Type type)
{
    free();
    m_type = type;
    switch (m_type)
    {
        case BCODE_INTEGER:
            m_value.m_int = 0;
            break;

        case BCODE_STRING:
            m_value.m_string = new std::string;
            break;

        case BCODE_LIST:
        case BCODE_DICTIONARY:
            m_value.m_map = new DictionaryValues();
            break;

        default:
            ASSERT_MSG(false, "type(" + std::to_string(m_type) + ") is no support");
            break;
    }
}

void BCodeValue::clear()
{
    ASSERT_MSG(m_type == BCODE_LIST || m_type == BCODE_DICTIONARY, "in BCodeValue::clear(): requires list or dictionary value");
    m_value.m_map->clear();
}

void BCodeValue::resize(size_t newSize)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in BCodeValue::resize(): requires list value");

    size_t oldSize = size();
    if (newSize == 0)
        clear();
    else if (newSize > oldSize)
        this->operator[](newSize - 1);
    else
        for (size_t index = newSize; index < oldSize; index++)
            m_value.m_map->erase(index);
}

BCodeValue& BCodeValue::operator[](size_t index)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in BCodeValue::operator[](size_t index): requires list value");
    CZString key(index);
    auto it = m_value.m_map->lower_bound(key);
    if (it != m_value.m_map->end() && (*it).first == key)
        return (*it).second;

    DictionaryValues::value_type defaultValue(key, BCodeValue(BCODE_STRING));
    it = m_value.m_map->insert(it, defaultValue);
    return (*it).second;
}

BCodeValue& BCodeValue::operator[](const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::operator[](const std::string& key): requires dictionary value");

    CZString actualKey(key);
    auto it = m_value.m_map->lower_bound(actualKey);
    if (it != m_value.m_map->end() && (*it).first == actualKey)
        return (*it).second;

    DictionaryValues::value_type defaultValue(actualKey, BCodeValue(BCODE_STRING));
    it = m_value.m_map->insert(it, defaultValue);
    return (*it).second;
}

void BCodeValue::swap(BCodeValue& other)
{
    std::swap(m_type, other.m_type);
    std::swap(m_value, other.m_value);
}

BCodeValue& BCodeValue::operator=(const BCodeValue& other)
{
    BCodeValue(other).swap(*this);
    return *this;
}

bool BCodeValue::operator<(const BCodeValue& other) const
{
    int typeDelta = m_type - other.m_type;
    if (typeDelta)
        return typeDelta < 0;

    switch (m_type)
    {
        case BCODE_INTEGER:
        {
            return m_value.m_int < other.m_value.m_int;
        }

        case BCODE_STRING:
        {
            if ((m_value.m_string == nullptr) || (other.m_value.m_string == nullptr))
                return other.m_value.m_string != nullptr;
            return *m_value.m_string < *other.m_value.m_string;
        }

        case BCODE_LIST:
        case BCODE_DICTIONARY:
        {
            auto thisSize = m_value.m_map->size();
            auto otherSize = other.m_value.m_map->size();
            if (thisSize != otherSize)
                return thisSize < otherSize;
            return (*m_value.m_map) < (*other.m_value.m_map);
        }

        default:
            return false;
    }
}

bool BCodeValue::operator>(const BCodeValue& other) const
{
    return other < *this;
}

bool BCodeValue::operator<=(const BCodeValue& other) const
{
    return !(other < *this);
}

bool BCodeValue::operator>=(const BCodeValue& other) const
{
    return !(*this < other);
}

bool BCodeValue::operator==(const BCodeValue& other) const
{
    if (m_type != other.m_type)
        return false;

    switch (m_type)
    {
        case BCODE_INTEGER:
            return m_value.m_int == other.m_value.m_int;

        case BCODE_STRING:
            if ((m_value.m_string == nullptr) || (other.m_value.m_string == nullptr))
                return m_value.m_string == other.m_value.m_string;
            return *m_value.m_string < *other.m_value.m_string;

        case BCODE_LIST:
        case BCODE_DICTIONARY:
            return m_value.m_map->size() == other.m_value.m_map->size() && (*m_value.m_map) == (*other.m_value.m_map);

        default:
            return false;
    }
}

bool BCodeValue::operator!=(const BCodeValue& other) const
{
    return !(*this == other);
}

int64_t BCodeValue::asInt() const
{
    if (m_type == BCODE_INTEGER)
        return m_value.m_int;

    ASSERT_MSG(false, "Value is not convertible to Int");
    return 0;
}

std::string BCodeValue::asString() const
{
    switch (m_type)
    {
        case BCODE_INTEGER:
            return std::to_string(m_value.m_int);

        case BCODE_STRING:
            if (m_value.m_string == nullptr)
                return "";
            return *m_value.m_string;

        default:
            ASSERT_MSG(false, "Type is not convertible to string");
            return "";
    }
}

BCodeValue& BCodeValue::append(const BCodeValue& value)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in BCodeValue::append: requires list value");
    return this->m_value.m_map->emplace(size(), BCodeValue(value)).first->second;
}

bool BCodeValue::insert(size_t index, const BCodeValue& newValue)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in BCodeValue::insert: requires list value");
    size_t length = size();
    if (index > length)
        return false;

    for (size_t i = length; i > index; i--)
        (*this)[i] = std::move((*this)[i - 1]);

    (*this)[index] = BCodeValue(newValue);
    return true;
}

BCodeValue BCodeValue::get(size_t index, const BCodeValue& defaultValue) const
{
    ASSERT_MSG(m_type == BCODE_LIST, "in BCodeValue::get(size_t index, const BCodeValue& defaultValue): requires list value");
    CZString key(index);
    auto it = m_value.m_map->lower_bound(key);
    if (it != m_value.m_map->end() && (*it).first == key)
        return (*it).second;
    return defaultValue;
}

BCodeValue BCodeValue::get(const std::string& key, const BCodeValue& defaultValue) const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::get(const std::string& key, const BCodeValue& defaultValue): requires dictionary value");

    CZString actualKey(key);
    auto it = m_value.m_map->lower_bound(actualKey);
    if (it != m_value.m_map->end() && (*it).first == actualKey)
        return (*it).second;
    return defaultValue;
}

BCodeValue::const_iterator BCodeValue::find(const std::string& key) const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::::find(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    DictionaryValues::iterator it = m_value.m_map->find(actualKey);
    return const_iterator(it);
}

BCodeValue::iterator BCodeValue::find(const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::::find(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    DictionaryValues::iterator it = m_value.m_map->find(actualKey);
    return iterator(it);
}

bool BCodeValue::isMember(const std::string& key) const
{
    return find(key) != end();
}

void BCodeValue::removeMember(const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::removeMember(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    m_value.m_map->erase(actualKey);
}

bool BCodeValue::removeMember(const std::string& key, BCodeValue* removed)
{
    if (m_type != BCODE_DICTIONARY)
        return false;

    CZString actualKey(key);
    auto it = m_value.m_map->find(actualKey);
    if (it == m_value.m_map->end())
        return false;
    if (removed)
        *removed = std::move(it->second);
    m_value.m_map->erase(it);
    return true;
}

bool BCodeValue::removeIndex(size_t index, BCodeValue* removed)
{
    if (m_type != BCODE_DICTIONARY)
        return false;

    CZString key(index);
    auto it = m_value.m_map->find(key);
    if (it == m_value.m_map->end())
        return false;

    if (removed)
        *removed = it->second;

    size_t oldSize = size();
    for (size_t i = index; i < (oldSize - 1); i++)
    {
        CZString keey(i);
        (*m_value.m_map)[keey] = (*this)[i + 1];
    }
    CZString keyLast(oldSize - 1);
    auto itLast = m_value.m_map->find(keyLast);
    m_value.m_map->erase(itLast);
    return true;
}

std::vector<std::string> BCodeValue::getMemberNames() const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in BCodeValue::getMemberNames(): requires dictionary value");

    std::vector<std::string> members;
    members.reserve(m_value.m_map->size());
    DictionaryValues::const_iterator it = m_value.m_map->begin();
    DictionaryValues::const_iterator itEnd = m_value.m_map->end();
    for (; it != itEnd; it++)
        members.push_back(*it->first.data());
    return std::move(members);
}

BCodeValue::const_iterator BCodeValue::begin() const
{
    switch (m_type)
    {
        case BCODE_LIST:
        case BCODE_DICTIONARY:
            if (m_value.m_map)
                return const_iterator(m_value.m_map->begin());
            break;
        default:
            break;
    }
    return {};
}

BCodeValue::const_iterator BCodeValue::end() const
{
    switch (m_type)
    {
        case BCODE_LIST:
        case BCODE_DICTIONARY:
            if (m_value.m_map)
              return const_iterator(m_value.m_map->end());
            break;
        default:
            break;
    }
    return {};
}

BCodeValue::iterator BCodeValue::begin()
{
    switch (m_type)
    {
        case BCODE_LIST:
        case BCODE_DICTIONARY:
            if (m_value.m_map)
                return iterator(m_value.m_map->begin());
            break;
        default:
            break;
    }
    return iterator();
}

BCodeValue::iterator BCodeValue::end()
{
    switch (m_type)
    {
        case BCODE_LIST:
        case BCODE_DICTIONARY:
            if (m_value.m_map)
                return iterator(m_value.m_map->end());
            break;
        default:
            break;
    }
    return iterator();
}

void BCodeValue::free()
{
    switch (m_type)
    {
        case BCODE_INTEGER:
            m_value.m_int = 0;
            break;

        case BCODE_STRING:
            delete m_value.m_string;
            m_value.m_string = nullptr;
            break;

        case BCODE_LIST:
        case BCODE_DICTIONARY:
            delete m_value.m_map;
            m_value.m_map = nullptr;
            break;

        default:
            ASSERT_MSG(false, "type(" + std::to_string(m_type) + ") is no support");
            break;
    }
}
////////// BCodeValue


////////// BCodeValueIteratorBase
BCodeValueIteratorBase::BCodeValueIteratorBase()
    :m_current()
{
}

BCodeValueIteratorBase::BCodeValueIteratorBase(const BCodeValue::DictionaryValues::iterator& current)
    :m_current(current), m_isNull(false)
{
}

BCodeValue BCodeValueIteratorBase::key() const
{
    const BCodeValue::CZString str = (*m_current).first;
    if (str.data())
        return BCodeValue(*str.data());
    return BCodeValue(str.index());
}

size_t BCodeValueIteratorBase::index() const
{
    const BCodeValue::CZString str = (*m_current).first;
    if (!str.data())
        return str.index();
    return (size_t)-1;
}

std::string BCodeValueIteratorBase::name() const
{
    std::string* cname = m_current->first.data();
    return cname == nullptr ? "" : *cname;
}

int64_t BCodeValueIteratorBase::computeDistance(const SelfType& other) const
{
    if (m_isNull && other.m_isNull)
        return 0;

    int64_t distance = 0;
    for (BCodeValue::DictionaryValues::iterator it = m_current; it != other.m_current; it++)
        distance++;

    return distance;
}

bool BCodeValueIteratorBase::isEqual(const SelfType& other) const
{
    if (m_isNull)
        return other.m_isNull;
    return m_current == other.m_current;
}

void BCodeValueIteratorBase::copy(const SelfType& other)
{
    m_current = other.m_current;
    m_isNull = other.m_isNull;
}
////////// BCodeValueIteratorBase


////////// BCodeValueConstIterator
BCodeValueConstIterator::BCodeValueConstIterator() = default;

BCodeValueConstIterator::BCodeValueConstIterator(const BCodeValueIterator & other)
    :BCodeValueIteratorBase(other)
{
}

// private
BCodeValueConstIterator::BCodeValueConstIterator(const BCodeValue::DictionaryValues::iterator& current)
    :BCodeValueIteratorBase(current)
{
}
////////// BCodeValueConstIterator


////////// BCodeValueIterator
BCodeValueIterator::BCodeValueIterator() = default;

BCodeValueIterator::BCodeValueIterator(const SelfType& other) = default;

BCodeValueIterator::BCodeValueIterator(const BCodeValueConstIterator& other)
    :BCodeValueIteratorBase(other)
{
    ASSERT_MSG(false, "ConstIterator to Iterator should never be allowed");
}

// private
BCodeValueIterator::BCodeValueIterator(const BCodeValue::DictionaryValues::iterator& current)
    :BCodeValueIteratorBase(current)
{
}
////////// BCodeValueIterator

// static
bool BEncoding::decode_from_file(const std::string& filename, BCodeValue* root)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        log_error << "open file(" << filename << ") fail";
        return false;
    }
    ifs.seekg(0, std::ios::end);
    size_t len = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char* p = new char[len];
    ifs.read(p, len);
    ifs.close();
    size_t start = 0;
    bool ret = BEncoding::decode(p, start, len, root);
    delete p;
    return ret;
}

// static
bool BEncoding::decode(const std::string& str, BCodeValue* root)
{
    size_t start = 0;
    return BEncoding::decode(str.c_str(), start, str.size(), root);
}

// static
bool BEncoding::decode(const char* str, size_t& begin, const size_t& end, BCodeValue* root)
{
#define RET() \
    log_error << begin; \
    return false

    BCodeValue::Type type;
    if (str[begin] == 'l') // 列表类型
    {
        type = BCodeValue::BCODE_LIST;
        root->setType(BCodeValue::BCODE_LIST);
    }
    else if (str[begin] == 'd') // 字典类型
    {
        type = BCodeValue::BCODE_DICTIONARY;
        root->setType(BCodeValue::BCODE_DICTIONARY);
    }
    else
    {
        RET();
    }

    begin++; // 跳过开头的l或者d
    while (begin < end)
    {
        switch (str[begin])
        {
            case 'e': // 表示当前列表或者字典数据已经读取完毕
            {
                begin++; // 跳过结尾的e
                return true;
            }

            case 'i': // 整型数
            {
                if (type == BCodeValue::BCODE_DICTIONARY) // 字典的形式(<string, object>)
                {
                    RET();
                }
                begin++; // 跳过开头的i
                std::string num_str;
                while (begin < end && str[begin] != 'e')
                {
                    num_str.append(1, str[begin]);
                    begin++;
                }
                if (begin >= end)
                {
                    RET();
                }
                int64_t value = atol(num_str.c_str());
                BCodeValue tmp(value);
                root->append(tmp);
                begin++; // 跳过结尾的e
                break;
            }

            case 'l': // 列表
            case 'd': // 字典
            {
                if (type == BCodeValue::BCODE_DICTIONARY) // 字典的形式(<string, object>)
                {
                    RET();
                }

                BCodeValue tmp;
                if (!BEncoding::decode(str, begin, end, &tmp))
                {
                    RET();
                }

                root->append(tmp);
                break;
            }

            default: // 字符串(需根据当前是列表还是字典进行数据插入操作)
            {
                std::string name_len;
                while (begin < end && str[begin] != ':')
                {
                    name_len.append(1, str[begin]);
                    begin++;
                }
                if (begin >= end)
                {
                    RET();
                }
                begin++; // 跳过 :
                size_t len = atol(name_len.c_str());
                if (begin + len >= end)
                {
                    RET();
                }
                std::string name(&str[begin], len);
                //log_error << "name: " << name;
                begin += len;
                if (type == BCodeValue::BCODE_LIST)
                {
                    BCodeValue tmp(name);
                    root->append(tmp);
                }
                else if (type == BCodeValue::BCODE_DICTIONARY)
                {
                    if (str[begin] == 'i')
                    {
                        begin++; // 跳过开头的i
                        std::string num_str;
                        while (begin < end && str[begin] != 'e')
                        {
                            num_str.append(1, str[begin]);
                            begin++;
                        }
                        if (begin >= end)
                        {
                            RET();
                        }
                        int64_t value = atol(num_str.c_str());
                        BCodeValue tmp(value);
                        (*root)[name] = tmp;
                        begin++; // 跳过结尾的e
                    }
                    else if (str[begin] == 'l' || str[begin] == 'd')
                    {
                        BCodeValue tmp;
                        if (!BEncoding::decode(str, begin, end, &tmp))
                        {
                            RET();
                        }
                        (*root)[name] = tmp;
                    }
                    else
                    {
                        name_len.clear();
                        while (begin < end && str[begin] != ':')
                        {
                            name_len.append(1, str[begin]);
                            begin++;
                        }
                        if (begin >= end)
                        {
                            RET();
                        }
                        begin++; // 跳过 :
                        len = atol(name_len.c_str());
                        if (begin + len >= end)
                        {
                            RET();
                        }
                        std::string value(&str[begin], len);
                        begin += len;
                        BCodeValue tmp(value);
                        (*root)[name] = tmp;
                    }
                }
                break;
            }
        }
    }

    RET();

#undef RET
}

// static
bool BEncoding::encode_to_file(const std::string& filename, BCodeValue* root)
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
    {
        log_error << "open file(" << filename << ") fail";
        return false;
    }
    return BEncoding::encode(ofs, root);
}

// static
bool BEncoding::encode(std::string& str, BCodeValue* root)
{
    std::stringstream ss;
    bool ret = BEncoding::encode(ss, root);
    if (ret)
        str = std::move(ss.str());
    return ret;
}

// static
bool BEncoding::encode(std::ostream& os, BCodeValue* root)
{
    switch (root->getType())
    {
        case BCodeValue::BCODE_INTEGER:
        {
            os << "i" << root->asInt() << "e";
            break;
        }

        case BCodeValue::BCODE_STRING:
        {
            std::string str = root->asString();
            size_t len = str.size();
            os << std::to_string(len) << ":" << str;
            break;
        }

        case BCodeValue::BCODE_LIST:
        {
            os << "l";
            for (size_t i = 0; i < root->size(); i++)
                BEncoding::encode(os, &(*root)[i]);
            os << "e";
            break;
        }

        case BCodeValue::BCODE_DICTIONARY:
        {
            os << "d";
            std::vector<std::string> names = root->getMemberNames();
            for (std::string& i : names)
            {
                size_t len = i.size();
                os << std::to_string(len) << ":" << i;
                BEncoding::encode(os, &(*root)[i]);
            }
            os << "e";
            break;
        }

        default:
        {
            return false;
        }
    }

    return true;
}

}
