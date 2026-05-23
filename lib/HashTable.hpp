#pragma once
#include "AVL.hpp"
#include <string>
#include <functional>
#include <sstream>

template <typename K>
struct Hash
{
    unsigned long operator()(const K &key) const
    {
        std::stringstream ss;
        ss << key;
        std::string strKey = ss.str();

        unsigned long hash = 5381;
        for (char c : strKey)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
};

template <>
struct Hash<std::string>
{
    unsigned long operator()(const std::string &key) const
    {
        unsigned long hash = 5381;
        for (char c : key)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
};

template <typename K, typename V>
struct HashTable
{
private:
    struct HashEntry
    {
        K key;
        V value;

        HashEntry() {} // <-- Đã thêm khởi tạo rỗng

        HashEntry(K k, V v)
        {
            key = k;
            value = v;
        }
    };

    struct KeyCompare
    {
        bool operator()(const HashEntry &a, const HashEntry &b) const
        {
            return a.key < b.key;
        }
    };

    AVL<HashEntry, KeyCompare> *table;
    int capacity;
    int currentSize;

    int hashFunc(const K &key) const
    {
        Hash<K> hasher;
        return hasher(key) % capacity;
    };

public:
    HashTable(int cap)
    {
        capacity = cap;
        currentSize = 0;
        table = new AVL<HashEntry, KeyCompare>[cap];
    }

    ~HashTable()
    {
        delete[] table;
    }

    void insert(const K &key, const V &value)
    {
        int index = hashFunc(key);
        table[index].insert(HashEntry(key, value));
        currentSize++;
    }

    V *find(const K &key)
    {
        int index = hashFunc(key);
        HashEntry tempBox = HashEntry(key, V());
        HashEntry *temp = table[index].search(tempBox);
        if (temp == nullptr)
            return nullptr;
        return &(temp->value);
    }

    void remove(const K &key)
    {
        int index = hashFunc(key);
        HashEntry tempBox = HashEntry(key, V());
        HashEntry *temp = table[index].search(tempBox);
        if (temp == nullptr)
            return;
        else
        {
            table[index].remove(tempBox);
            currentSize--;
        }
    }

    int size() const
    {
        return currentSize;
    }

    bool contains(const K &key) const
    {
        int index = hashFunc(key);
        HashEntry tempBox = HashEntry(key, V());
        HashEntry *temp = table[index].search(tempBox);
        return (temp != nullptr);
    }

    void clear()
    {
        for (int i = 0; i < capacity; i++)
        {
            table[i].clear();
        }
        currentSize = 0;
    }
};