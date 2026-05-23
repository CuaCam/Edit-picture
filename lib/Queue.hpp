#pragma once
#include <iostream>
using namespace std;

template <typename T>
class Queue
{
private:
    T *data = nullptr;
    int capacity;
    int count;
    int front;
    int rear;

    // Thay doi kich thuoc mang chua Queue
    void resize(int newCapacity)
    {
        T *temp = new T[newCapacity];
        for (int i = 0; i < count; i++)
        {
            temp[i] = data[(front + i) % capacity];
        }
        delete[] data;
        data = temp;
        capacity = newCapacity;
        front = 0;
        rear = count;
    }

public:
    // Check Queue rong "q.empty();"
    bool empty() const
    {
        return count == 0;
    }

    // Tao Queue "Queue<kieu_du_lieu> ten_queue;"
    Queue(int initialCapacity = 4)
    {
        capacity = initialCapacity;
        data = new T[initialCapacity];
        count = 0;
        front = 0;
        rear = 0;
    }

    // Xoa Queue "q.clear();"
    void clear()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
        count = 0;
        capacity = 0;
        front = 0;
        rear = 0;
    }

    // Tranh memory leaking
    ~Queue()
    {
        clear();
    }

    // Them phan tu vao Queue "q.enqueue(val);"
    void enqueue(T val)
    {
        if (count == capacity)
        {
            int newCapacity = (capacity == 0) ? 4 : capacity * 2;
            resize(newCapacity);
        }
        data[rear] = val;
        rear = (rear + 1) % capacity;
        count++;
    }

    // Xoa phan tu khoi Queue "q.dequeue();"
    void dequeue()
    {
        if (count > 0)
        {
            front = (front + 1) % capacity;
            count--;
        }
    }

    // Tra ve tham chieu de co the thay đoi du lieu phan tu dau
    T &getFront()
    {
        if (empty())
        {
            throw runtime_error("Queue is empty");
        }
        return data[front];
    }

    // Tra ve tham chieu heng (chi doc) dung cho cac object const
    const T &getFront() const
    {
        if (empty())
        {
            throw runtime_error("Queue is empty");
        }
        return data[front];
    }

    // Kich thuoc "q.size();"
    int size() const
    {
        return count;
    }
};