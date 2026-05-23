#pragma once
#include <iostream>

using namespace std;

template <typename T>
class Stack
{
private:
    T *data = nullptr;
    int topIndex = -1;
    int capacity = 0;

    // Thay doi kich thuoc
    void resize(int newCapacity)
    {
        T *temp = new T[newCapacity];
        for (int i = 0; i <= topIndex; i++)
        {
            temp[i] = data[i];
        }
        delete[] data;
        data = temp;
        capacity = newCapacity;
    }

public:
    // Kiem tra stack co rong khong "s.empty();"
    bool empty()
    {
        return topIndex == -1;
    }

    // Tao stack "Stack<kieu_du_lieu> ten_Stack;"
    Stack(int initialCapacity = 4)
    {
        capacity = initialCapacity;
        topIndex = -1;
        data = new T[capacity];
    }

    // Tranh memory leaking
    ~Stack()
    {
        clear();
    }

    // Xoa bo stack "s.clear();"
    void clear()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
        topIndex = -1;
        capacity = 0;
    }

    // Them phan tu vao stack
    void push(T val)
    {
        if (topIndex >= capacity - 1)
        {
            int newCapacity = (capacity == 0) ? 4 : capacity * 2;
            resize(newCapacity);
        }
        topIndex++;
        data[topIndex] = val;
    }

    // Xoa phan tu khoi stack "s.pop();"
    void pop()
    {
        if (empty())
            return;
        topIndex--;
        if (topIndex >= 0 && topIndex < capacity / 4)
        {
            resize(capacity / 2);
        }
    }

    // Gia tri phan tu cuoi cua stack "s.top();" - Phiên bản Đọc & Ghi
    T &top()
    {
        if (empty())
        {
            throw runtime_error("Stack is empty");
        }
        return data[topIndex];
    }

    // Gia tri phan tu cuoi cua stack "s.top();" - Phiên bản Chỉ đọc (const)
    const T &top() const
    {
        if (empty())
        {
            throw runtime_error("Stack is empty");
        }
        return data[topIndex];
    }

    // Kiem tra so phan tu trong stack s.size();
    int size()
    {
        return topIndex + 1;
    }
};