/**
 * brief: B编码解析类
 */
#ifndef __BIFANG_BENCODE_H
#define __BIFANG_BENCODE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace bifang
{

namespace BEncode
{

class ValueConstIterator;
class ValueIterator;

class Value
{
friend class ValueIteratorBase;

private:
    class CZString
    {
    public:
        CZString(size_t index);
        CZString(const std::string& str);
        CZString(const CZString& other);
        ~CZString();

        CZString& operator=(const CZString& other);
        
        bool operator<(const CZString& other) const;
        bool operator==(const CZString& other) const;

        std::string* data() const { return m_str;   }
        size_t index()      const { return m_index; }

    private:
        std::string* m_str;
        size_t m_index;
    };

public:
    typedef std::map<CZString, Value> DictionaryValues;
    using const_iterator = ValueConstIterator;
    using iterator = ValueIterator;

    enum Type
    {
        BCODE_INTEGER = 0,
        BCODE_STRING,
        BCODE_LIST,
        BCODE_DICTIONARY,
    };

    Value(Type type = BCODE_INTEGER);
    Value(int64_t value);
    Value(const std::string& value);
    Value(const Value& other);
    ~Value();

    Type getType()      const { return m_type;                     }
    bool isInteger()    const { return m_type == BCODE_INTEGER;    }
    bool isString()     const { return m_type == BCODE_STRING;     }
    bool isList()       const { return m_type == BCODE_LIST;       }
    bool isDictionary() const { return m_type == BCODE_DICTIONARY; }
    size_t size() const;
    bool empty() const;
    std::string typeToString() const;

    void setType(Type type);
    void clear();
    void resize(size_t newSize);

    /**
     * brief: You may need to say 'value[0u]' to get your compiler to distinguish
     *        this from the operator[] which takes a string
     */
    Value& operator[](size_t index);
    Value& operator[](const std::string& key);

    void swap(Value& other);
    Value& operator=(const Value& other);

    bool operator<(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>=(const Value& other) const;
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;

    int64_t asInt() const;
    std::string asString() const;

    Value& append(const Value& value);
    bool insert(size_t index, const Value& newValue);

    Value get(size_t index, const Value& defaultValue) const;
    Value get(const std::string& key, const Value& defaultValue) const;

    const_iterator find(const std::string& key) const;
    iterator find(const std::string& key);

    bool isMember(const std::string& key) const;
    void removeMember(const std::string& key);
    bool removeMember(const std::string& key, Value* removed);
    bool removeIndex(size_t index, Value* removed);
    std::vector<std::string> getMemberNames() const;

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

private:
    void free();

private:
    Type m_type;
    union ValueHolder
    {
        int64_t m_int;
        std::string* m_string;
        DictionaryValues* m_map;
    } m_value;
};

/**
 * brief: Value 迭代器基类
 */
class ValueIteratorBase
{
public:
    using SelfType = ValueIteratorBase;

    ValueIteratorBase();
    explicit ValueIteratorBase(const Value::DictionaryValues::iterator& current);

public:
    bool operator==(const SelfType& other) const { return isEqual(other);  }
    bool operator!=(const SelfType& other) const { return !isEqual(other); }
    int64_t operator-(const SelfType& other) const { return other.computeDistance(*this); }

    Value key() const;

    size_t index() const;

    std::string name() const;

protected:
    Value& deref() { return m_current->second; }
    const Value& deref() const { return m_current->second; }

    void increment() { m_current++; }
    void decrement() { m_current--; }

    int64_t computeDistance(const SelfType& other) const;

    bool isEqual(const SelfType& other) const;

    void copy(const SelfType& other);

private:
    Value::DictionaryValues::iterator m_current;
    bool m_isNull = true;
};

/**
 * brief: Value const迭代器
 */
class ValueConstIterator : public ValueIteratorBase
{
friend class Value;

public:
    using value_type = const Value;
    using reference = const Value&;
    using pointer = const Value*;
    using SelfType = ValueConstIterator;

    ValueConstIterator();
    ValueConstIterator(const ValueIterator & other);

private:
    explicit ValueConstIterator(const Value::DictionaryValues::iterator& current);

public:
    SelfType& operator=(const ValueIteratorBase& other)
    {
        copy(other);
        return *this;
    }

    SelfType operator++(int)
    {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int)
    {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType& operator--()
    {
        decrement();
        return *this;
    }

    SelfType& operator++()
    {
        increment();
        return *this;
    }

    reference operator*() const { return deref(); }

    pointer operator->() const { return &deref(); }
};

/**
 * brief: Value 迭代器
 */
class ValueIterator : public ValueIteratorBase
{
friend class Value;

public:
    using value_type = Value;
    using reference = Value&;
    using pointer = Value*;
    using SelfType = ValueIterator;

    ValueIterator();
    ValueIterator(const SelfType& other);
    explicit ValueIterator(const ValueConstIterator& other);

private:
    explicit ValueIterator(const Value::DictionaryValues::iterator& current);

public:
    SelfType& operator=(const SelfType& other)
    {
        copy(other);
        return *this;
    }

    SelfType operator++(int)
    {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int)
    {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType& operator--()
    {
        decrement();
        return *this;
    }

    SelfType& operator++()
    {
        increment();
        return *this;
    }

    reference operator*() { return deref(); }

    pointer operator->() { return &deref(); }
};

/**
 * brief: 从filename中获取内容, 再解码内容到root里
 * param: filename 文件名(完整路径)
 *        root 存储解析完成后B编码对象指针
 */
int64_t decode_from_file(const std::string& filename, Value* root);
/**
 * brief: 从str解码内容到root里
 * param: str 需解析字符串
 *        root 存储解析完成后B编码对象指针
 */
int64_t decode(const std::string& str, Value* root);
/**
 * brief: 从str解码内容到root里
 * param: str 需解析字符串
 *        begin 解析字符串起始位置(指针)
 *        end 解析字符串结束位置
 *        root 存储解析完成后B编码对象指针
 */
int64_t decode(const char* str, size_t& begin, const size_t& end, Value* root);

/**
 * brief: 从root里的内容使用b编码写到文件里
 * param: filename 文件名(完整路径)
 *        root 需写进文件的B编码对象指针
 */
bool encode_to_file(const std::string& filename, Value* root);
/**
 * brief: 从root里的内容加密到文件里
 * param: filename 文件名(完整路径)
 *        root 需写进文件的B编码对象指针
 */
bool encode(std::string& str, Value* root);
/**
 * brief: 从root里的内容加密到文件里
 * param: os 输出流
 *        root 需写进文件的B编码对象指针
 */
bool encode(std::ostream& os, Value* root);

std::string print(Value* root, size_t level = 1);

}

}

#endif /*__BIFANG_BENCODE_H*/
