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

class BCodeValueConstIterator;
class BCodeValueIterator;

class BCodeValue
{
friend class BCodeValueIteratorBase;

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
    typedef std::map<CZString, BCodeValue> DictionaryValues;
    using const_iterator = BCodeValueConstIterator;
    using iterator = BCodeValueIterator;

    enum Type
    {
        BCODE_INTEGER = 0,
        BCODE_STRING,
        BCODE_LIST,
        BCODE_DICTIONARY,
    };

    BCodeValue(Type type = BCODE_INTEGER);
    BCodeValue(int64_t value);
    BCodeValue(const std::string& value);
    BCodeValue(const BCodeValue& other);
    ~BCodeValue();

    Type getType()           const { return m_type;                     }
    bool isInteger()         const { return m_type == BCODE_INTEGER;    }
    bool isString()          const { return m_type == BCODE_STRING;     }
    bool isList()            const { return m_type == BCODE_LIST;       }
    bool isDictionary()      const { return m_type == BCODE_DICTIONARY; }
    size_t size() const;
    bool empty() const;

    void setType(Type type);
    void clear();
    void resize(size_t newSize);

    /**
     * brief: You may need to say 'value[0u]' to get your compiler to distinguish
     *        this from the operator[] which takes a string
     */
    BCodeValue& operator[](size_t index);
    BCodeValue& operator[](const std::string& key);

    void swap(BCodeValue& other);
    BCodeValue& operator=(const BCodeValue& other);

    bool operator<(const BCodeValue& other) const;
    bool operator>(const BCodeValue& other) const;
    bool operator<=(const BCodeValue& other) const;
    bool operator>=(const BCodeValue& other) const;
    bool operator==(const BCodeValue& other) const;
    bool operator!=(const BCodeValue& other) const;

    int64_t asInt() const;
    std::string asString() const;

    BCodeValue& append(const BCodeValue& value);
    bool insert(size_t index, const BCodeValue& newValue);

    BCodeValue get(size_t index, const BCodeValue& defaultValue) const;
    BCodeValue get(const std::string& key, const BCodeValue& defaultValue) const;

    const_iterator find(const std::string& key) const;
    iterator find(const std::string& key);

    bool isMember(const std::string& key) const;
    void removeMember(const std::string& key);
    bool removeMember(const std::string& key, BCodeValue* removed);
    bool removeIndex(size_t index, BCodeValue* removed);
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
 * brief: BCodeValue 迭代器基类
 */
class BCodeValueIteratorBase
{
public:
    using SelfType = BCodeValueIteratorBase;

    BCodeValueIteratorBase();
    explicit BCodeValueIteratorBase(const BCodeValue::DictionaryValues::iterator& current);

public:
    bool operator==(const SelfType& other) const { return isEqual(other);  }
    bool operator!=(const SelfType& other) const { return !isEqual(other); }
    int64_t operator-(const SelfType& other) const { return other.computeDistance(*this); }

    BCodeValue key() const;

    size_t index() const;

    std::string name() const;

protected:
    BCodeValue& deref() { return m_current->second; }
    const BCodeValue& deref() const { return m_current->second; }

    void increment() { m_current++; }
    void decrement() { m_current--; }

    int64_t computeDistance(const SelfType& other) const;

    bool isEqual(const SelfType& other) const;

    void copy(const SelfType& other);

private:
    BCodeValue::DictionaryValues::iterator m_current;
    bool m_isNull = true;
};

/**
 * brief: BCodeValue const迭代器
 */
class BCodeValueConstIterator : public BCodeValueIteratorBase
{
friend class BCodeValue;

public:
    using value_type = const BCodeValue;
    using reference = const BCodeValue&;
    using pointer = const BCodeValue*;
    using SelfType = BCodeValueConstIterator;

    BCodeValueConstIterator();
    BCodeValueConstIterator(const BCodeValueIterator & other);

private:
    explicit BCodeValueConstIterator(const BCodeValue::DictionaryValues::iterator& current);

public:
    SelfType& operator=(const BCodeValueIteratorBase& other)
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
 * brief: BCodeValue 迭代器
 */
class BCodeValueIterator : public BCodeValueIteratorBase
{
friend class BCodeValue;

public:
    using value_type = BCodeValue;
    using reference = BCodeValue&;
    using pointer = BCodeValue*;
    using SelfType = BCodeValueIterator;

    BCodeValueIterator();
    BCodeValueIterator(const SelfType& other);
    explicit BCodeValueIterator(const BCodeValueConstIterator& other);

private:
    explicit BCodeValueIterator(const BCodeValue::DictionaryValues::iterator& current);

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

class BEncoding
{
public:
    /**
     * brief: 从filename中获取内容, 再解码内容到root里
     * param: filename 文件名(完整路径)
     *        root 存储解析完成后B编码对象指针
     */
    static bool decode_from_file(const std::string& filename, BCodeValue* root);

    /**
     * brief: 从str解码内容到root里
     * param: str 需解析字符串
     *        root 存储解析完成后B编码对象指针
     */
    static bool decode(const std::string& str, BCodeValue* root);

    /**
     * brief: 从str解码内容到root里
     * param: str 需解析字符串
     *        begin 解析字符串起始位置(指针)
     *        end 解析字符串结束位置
     *        root 存储解析完成后B编码对象指针
     */
    static bool decode(const char* str, size_t& begin, const size_t& end, BCodeValue* root);

    /**
     * brief: 从root里的内容使用b编码写到文件里
     * param: filename 文件名(完整路径)
     *        root 需写进文件的B编码对象指针
     */
    static bool encode_to_file(const std::string& filename, BCodeValue* root);

    /**
     * brief: 从root里的内容加密到文件里
     * param: filename 文件名(完整路径)
     *        root 需写进文件的B编码对象指针
     */
    static bool encode(std::string& str, BCodeValue* root);

    /**
     * brief: 从root里的内容加密到文件里
     * param: os 输出流
     *        root 需写进文件的B编码对象指针
     */
    static bool encode(std::ostream& os, BCodeValue* root);
};

}

#endif /*__BIFANG_BENCODE_H*/
