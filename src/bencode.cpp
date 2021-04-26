#include "bencode.h"
#include "log.h"
#include "Assert.h"

// UTF编码
namespace bifang
{

namespace BEncode
{

SystemLogger();

////////// Value::CZString 
Value::CZString::CZString(size_t index)
    :m_str(nullptr)
    ,m_index(index)
{
}

Value::CZString::CZString(const std::string& str)
{
    m_str = new std::string(str);
}

Value::CZString::CZString(const CZString& other)
{
    if (!other.m_str)
    {
        m_str = nullptr;
        m_index = other.m_index;
    }
    else
        m_str = new std::string(*other.m_str);
}

Value::CZString::~CZString()
{
    if (m_str)
        delete m_str;
}

Value::CZString& Value::CZString::operator=(const CZString& other)
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

bool Value::CZString::operator<(const CZString& other) const
{
    if (!m_str)
        return m_index < other.m_index;

    ASSERT_MSG(m_str && other.m_str, "Value::CZString::operator<(const CZString& other) couldn't compare different type");

    return *m_str < *other.m_str;
}

bool Value::CZString::operator==(const CZString& other) const
{
    if (!m_str)
        return m_index == other.m_index;

    ASSERT_MSG(m_str && other.m_str, "Value::CZString::operator==(const CZString& other) couldn't compare different type");

    return *m_str == *other.m_str;
}
////////// Value::CZString 


////////// Value
Value::Value(Value::Type type)
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

Value::Value(int64_t value)
{
    m_type = BCODE_INTEGER;
    m_value.m_int = value;
}

Value::Value(const std::string& value)
{
    m_type = BCODE_STRING;
    m_value.m_string = new std::string(value);
}

Value::Value(const Value& other)
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

Value::~Value()
{
    free();
}

size_t Value::size() const
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

bool Value::empty() const
{
    if (isList() || isDictionary())
        return size() == 0;
    return false;
}

std::string Value::typeToString() const
{
    switch (m_type)
    {
        case BCODE_INTEGER:
            return "i";
        case BCODE_STRING:
            return "b";
        case BCODE_LIST:
            return "l";
        case BCODE_DICTIONARY:
            return "d";
        default:
            return "";
    }
}

void Value::setType(Value::Type type)
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

void Value::clear()
{
    ASSERT_MSG(m_type == BCODE_LIST || m_type == BCODE_DICTIONARY, "in Value::clear(): requires list or dictionary value");
    m_value.m_map->clear();
}

void Value::resize(size_t newSize)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in Value::resize(): requires list value");

    size_t oldSize = size();
    if (newSize == 0)
        clear();
    else if (newSize > oldSize)
        this->operator[](newSize - 1);
    else
        for (size_t index = newSize; index < oldSize; index++)
            m_value.m_map->erase(index);
}

Value& Value::operator[](size_t index)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in Value::operator[](size_t index): requires list value");
    CZString key(index);
    auto it = m_value.m_map->lower_bound(key);
    if (it != m_value.m_map->end() && (*it).first == key)
        return (*it).second;

    DictionaryValues::value_type defaultValue(key, Value(BCODE_STRING));
    it = m_value.m_map->insert(it, defaultValue);
    return (*it).second;
}

Value& Value::operator[](const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::operator[](const std::string& key): requires dictionary value");

    CZString actualKey(key);
    auto it = m_value.m_map->lower_bound(actualKey);
    if (it != m_value.m_map->end() && (*it).first == actualKey)
        return (*it).second;

    DictionaryValues::value_type defaultValue(actualKey, Value(BCODE_STRING));
    it = m_value.m_map->insert(it, defaultValue);
    return (*it).second;
}

void Value::swap(Value& other)
{
    std::swap(m_type, other.m_type);
    std::swap(m_value, other.m_value);
}

Value& Value::operator=(const Value& other)
{
    Value(other).swap(*this);
    return *this;
}

bool Value::operator<(const Value& other) const
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

bool Value::operator>(const Value& other) const
{
    return other < *this;
}

bool Value::operator<=(const Value& other) const
{
    return !(other < *this);
}

bool Value::operator>=(const Value& other) const
{
    return !(*this < other);
}

bool Value::operator==(const Value& other) const
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

bool Value::operator!=(const Value& other) const
{
    return !(*this == other);
}

int64_t Value::asInt() const
{
    if (m_type == BCODE_INTEGER)
        return m_value.m_int;

    ASSERT_MSG(false, "Value is not convertible to Int");
    return 0;
}

std::string Value::asString() const
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

Value& Value::append(const Value& value)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in Value::append: requires list value");
    return this->m_value.m_map->emplace(size(), Value(value)).first->second;
}

bool Value::insert(size_t index, const Value& newValue)
{
    ASSERT_MSG(m_type == BCODE_LIST, "in Value::insert: requires list value");
    size_t length = size();
    if (index > length)
        return false;

    for (size_t i = length; i > index; i--)
        (*this)[i] = std::move((*this)[i - 1]);

    (*this)[index] = Value(newValue);
    return true;
}

Value Value::get(size_t index, const Value& defaultValue) const
{
    ASSERT_MSG(m_type == BCODE_LIST, "in Value::get(size_t index, const Value& defaultValue): requires list value");
    CZString key(index);
    auto it = m_value.m_map->lower_bound(key);
    if (it != m_value.m_map->end() && (*it).first == key)
        return (*it).second;
    return defaultValue;
}

Value Value::get(const std::string& key, const Value& defaultValue) const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::get(const std::string& key, const Value& defaultValue): requires dictionary value");

    CZString actualKey(key);
    auto it = m_value.m_map->lower_bound(actualKey);
    if (it != m_value.m_map->end() && (*it).first == actualKey)
        return (*it).second;
    return defaultValue;
}

Value::const_iterator Value::find(const std::string& key) const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::::find(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    DictionaryValues::iterator it = m_value.m_map->find(actualKey);
    return const_iterator(it);
}

Value::iterator Value::find(const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::::find(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    DictionaryValues::iterator it = m_value.m_map->find(actualKey);
    return iterator(it);
}

bool Value::isMember(const std::string& key) const
{
    return find(key) != end();
}

void Value::removeMember(const std::string& key)
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::removeMember(const std::string& key): requires dictionary value");
    CZString actualKey(key);
    m_value.m_map->erase(actualKey);
}

bool Value::removeMember(const std::string& key, Value* removed)
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

bool Value::removeIndex(size_t index, Value* removed)
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

std::vector<std::string> Value::getMemberNames() const
{
    ASSERT_MSG(m_type == BCODE_DICTIONARY, "in Value::getMemberNames(): requires dictionary value");

    std::vector<std::string> members;
    members.reserve(m_value.m_map->size());
    DictionaryValues::const_iterator it = m_value.m_map->begin();
    DictionaryValues::const_iterator itEnd = m_value.m_map->end();
    for (; it != itEnd; it++)
        members.push_back(*it->first.data());
    return std::move(members);
}

Value::const_iterator Value::begin() const
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

Value::const_iterator Value::end() const
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

Value::iterator Value::begin()
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

Value::iterator Value::end()
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

void Value::free()
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
////////// Value


////////// ValueIteratorBase
ValueIteratorBase::ValueIteratorBase()
    :m_current()
{
}

ValueIteratorBase::ValueIteratorBase(const Value::DictionaryValues::iterator& current)
    :m_current(current), m_isNull(false)
{
}

Value ValueIteratorBase::key() const
{
    const Value::CZString str = (*m_current).first;
    if (str.data())
        return Value(*str.data());
    return Value(str.index());
}

size_t ValueIteratorBase::index() const
{
    const Value::CZString str = (*m_current).first;
    if (!str.data())
        return str.index();
    return (size_t)-1;
}

std::string ValueIteratorBase::name() const
{
    std::string* cname = m_current->first.data();
    return cname == nullptr ? "" : *cname;
}

int64_t ValueIteratorBase::computeDistance(const SelfType& other) const
{
    if (m_isNull && other.m_isNull)
        return 0;

    int64_t distance = 0;
    for (Value::DictionaryValues::iterator it = m_current; it != other.m_current; it++)
        distance++;

    return distance;
}

bool ValueIteratorBase::isEqual(const SelfType& other) const
{
    if (m_isNull)
        return other.m_isNull;
    return m_current == other.m_current;
}

void ValueIteratorBase::copy(const SelfType& other)
{
    m_current = other.m_current;
    m_isNull = other.m_isNull;
}
////////// ValueIteratorBase


////////// ValueConstIterator
ValueConstIterator::ValueConstIterator() = default;

ValueConstIterator::ValueConstIterator(const ValueIterator & other)
    :ValueIteratorBase(other)
{
}

// private
ValueConstIterator::ValueConstIterator(const Value::DictionaryValues::iterator& current)
    :ValueIteratorBase(current)
{
}
////////// ValueConstIterator


////////// ValueIterator
ValueIterator::ValueIterator() = default;

ValueIterator::ValueIterator(const SelfType& other) = default;

ValueIterator::ValueIterator(const ValueConstIterator& other)
    :ValueIteratorBase(other)
{
    ASSERT_MSG(false, "ConstIterator to Iterator should never be allowed");
}

// private
ValueIterator::ValueIterator(const Value::DictionaryValues::iterator& current)
    :ValueIteratorBase(current)
{
}
////////// ValueIterator

int64_t decode_from_file(const std::string& filename, Value* root)
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
    int64_t ret = decode(p, start, len, root);
    if (ret)
    {
        int64_t error_begin = ret == -1 ? 0 : ret;
        int64_t error_len = len - error_begin;
        if (error_len > 40)
            error_len = 40;
        log_error << "file(" << filename << ") error begin:" << error_begin
            << ", errorstr:" << std::string(p + error_begin, error_len);
    }

    delete p;
    return ret;
}

int64_t decode(const std::string& str, Value* root)
{
    size_t start = 0;
    return decode(str.c_str(), start, str.size(), root);
}

int64_t decode(const char* str, size_t& begin, const size_t& end, Value* root)
{
    Value::Type type;
    if (str[begin] == 'l') // 列表类型
    {
        type = Value::BCODE_LIST;
        root->setType(Value::BCODE_LIST);
    }
    else if (str[begin] == 'd') // 字典类型
    {
        type = Value::BCODE_DICTIONARY;
        root->setType(Value::BCODE_DICTIONARY);
    }
    else
    {
        return begin ? begin : -1;
    }

    begin++; // 跳过开头的l或者d
    while (begin < end)
    {
        switch (str[begin])
        {
            case 'e': // 表示当前列表或者字典数据已经读取完毕
            {
                begin++; // 跳过结尾的e
                return 0;
            }

            case 'i': // 整型数
            {
                int64_t ret = begin;
                if (type == Value::BCODE_DICTIONARY) // 字典的形式(<string, object>)
                    return ret;
                begin++; // 跳过开头的i
                std::string num_str;
                while (begin < end && str[begin] != 'e' &&
                    ((str[begin] >= '0' && str[begin] <= '9') || str[begin] == '-'))
                {
                    num_str.append(1, str[begin]);
                    begin++;
                }
                if (begin >= end || str[begin] != 'e')
                    return ret;
                int64_t value = atol(num_str.c_str());
                Value tmp(value);
                root->append(tmp);
                begin++; // 跳过结尾的e
                break;
            }

            case 'l': // 列表
            case 'd': // 字典
            {
                int64_t ret = begin;
                if (type == Value::BCODE_DICTIONARY) // 字典的形式(<string, object>)
                    return ret;
                Value tmp;
                ret = decode(str, begin, end, &tmp);
                if (ret)
                    return ret;
                root->append(tmp);
                break;
            }

            default: // 字符串(需根据当前是列表还是字典进行数据插入操作)
            {
                int64_t ret = begin;
                std::string name_len;
                while (begin < end && str[begin] >= '0' && str[begin] <= '9')
                {
                    name_len.append(1, str[begin]);
                    begin++;
                }
                if (begin >= end || str[begin] != ':')
                    return ret;
                begin++; // 跳过 :
                int64_t len = atol(name_len.c_str());
                if (len <= 0)
                    return ret;
                if (begin + len >= end)
                    return ret;

                std::string name(&str[begin], len);
                begin += len;
                ret = begin;
                if (type == Value::BCODE_LIST)
                {
                    Value tmp(name);
                    root->append(tmp);
                }
                else if (type == Value::BCODE_DICTIONARY)
                {
                    if (str[begin] == 'i')
                    {
                        begin++; // 跳过开头的i
                        std::string num_str;
                        while (begin < end && str[begin] != 'e' &&
                            ((str[begin] >= '0' && str[begin] <= '9') || str[begin] == '-'))
                        {
                            num_str.append(1, str[begin]);
                            begin++;
                        }
                        if (begin >= end || str[begin] != 'e')
                            return ret;
                        int64_t value = atol(num_str.c_str());
                        Value tmp(value);
                        (*root)[name] = tmp;
                        begin++; // 跳过结尾的e
                    }
                    else if (str[begin] == 'l' || str[begin] == 'd')
                    {
                        Value tmp;
                        ret = decode(str, begin, end, &tmp);
                        if (ret)
                            return ret;
                        (*root)[name] = tmp;
                    }
                    else
                    {
                        name_len.clear();
                        while (begin < end && str[begin] >= '0' && str[begin] <= '9')
                        {
                            name_len.append(1, str[begin]);
                            begin++;
                        }
                        if (begin >= end || str[begin] != ':')
                            return ret;
                        begin++; // 跳过 :
                        len = atol(name_len.c_str());
                        if (len <= 0)
                            return ret;
                        if (begin + len >= end)
                            return ret;

                        std::string value(&str[begin], len);
                        begin += len;
                        Value tmp(value);
                        (*root)[name] = tmp;
                    }
                }
                break;
            }
        }
    }
    return begin ? begin : -1;
}

bool encode_to_file(const std::string& filename, Value* root)
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
    {
        log_error << "open file(" << filename << ") fail";
        return false;
    }
    return encode(ofs, root);
}

bool encode(std::string& str, Value* root)
{
    std::stringstream ss;
    bool ret = encode(ss, root);
    if (ret)
        str = std::move(ss.str());
    return ret;
}

bool encode(std::ostream& os, Value* root)
{
    switch (root->getType())
    {
        case Value::BCODE_INTEGER:
        {
            os << "i" << root->asInt() << "e";
            break;
        }

        case Value::BCODE_STRING:
        {
            std::string str = root->asString();
            size_t len = str.size();
            os << std::to_string(len) << ":" << str;
            break;
        }

        case Value::BCODE_LIST:
        {
            os << "l";
            for (size_t i = 0; i < root->size(); i++)
                encode(os, &(*root)[i]);
            os << "e";
            break;
        }

        case Value::BCODE_DICTIONARY:
        {
            os << "d";
            std::vector<std::string> names = root->getMemberNames();
            for (std::string& name : names)
            {
                size_t len = name.size();
                os << std::to_string(len) << ":" << name;
                encode(os, &(*root)[name]);
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

std::string print(Value* root, size_t level)
{
    std::string str;
    if (level == 1)
    {
        if (root->getType() == Value::BCODE_LIST)
            str += "[ROOT](l)";
        else if (root->getType() == Value::BCODE_DICTIONARY)
            str += "[ROOT](d)";
        else
            return "";
    }

    if (root->getType() == Value::BCODE_INTEGER)
    {
        str += "=" + root->asString() + "\n";
    }
    else if (root->getType() == Value::BCODE_STRING)
    {
        std::string s = root->asString();
        str += "[" + std::to_string(s.size()) + "]=" + s + "\n";
    }
    else if (root->getType() == Value::BCODE_LIST)
    {
        str += "[" + std::to_string(root->size()) + "]\n";
        std::string prefix;
        for (size_t i = 0; i < level; i++)
            prefix += "\t";
        for (size_t i = 0; i < root->size(); i++)
        {
            str += prefix + "ITEM " + std::to_string(i) + "(" + (*root)[i].typeToString() + ")";
            str += print(&(*root)[i], level + 1);
        }
    }
    else if (root->getType() == Value::BCODE_DICTIONARY)
    {
        str += "[" + std::to_string(root->size()) + "]\n";
        std::string prefix;
        for (size_t i = 0; i < level; i++)
            prefix += "\t";
        std::vector<std::string> names = root->getMemberNames();
        for (std::string& name : names)
        {
            str += prefix + name + "(" + (*root)[name].typeToString() + ")";
            str += print(&(*root)[name], level + 1);
        }
    }
    return str;
}

}

}
