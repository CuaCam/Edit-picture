#pragma once
#include <iostream>
using namespace std;

template <typename T>
class PriorityQueue
{
private:
    T *data;
    int capacity;
    int count;

    // Thay doi kich thuoc mang chua PriorityQueue
    void resize(int newCapacity)
    {
        T *temp = new T[newCapacity];
        for (int i = 0; i < count; i++)
        {
            temp[i] = data[i];
        }
        delete[] data;
        data = temp;
        capacity = newCapacity;
    }

    void SwapElements(int i, int j)
    {
        T temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

public:
    // Khoi tao PriorityQueue "PriorityQueue<kieu_du_lieu> ten_PriorityQueue;"
    PriorityQueue(int initialCapacity = 4)
    {
        capacity = initialCapacity;
        data = new T[capacity];
        count = 0;
    }

    // Tranh memory leaking
    ~PriorityQueue()
    {
        delete[] data;
    }

    // Them phan tu vao PriorityQueue "pq.insert(val);"
    void insert(T val)
    {
        if (count >= capacity)
        {
            int newCapacity = (capacity == 0) ? 4 : capacity * 2;
            resize(newCapacity);
        }
        int currentIndex = count;
        data[currentIndex] = val;
        count++;

        while (currentIndex > 0)
        {
            int parentIndex = (currentIndex - 1) / 2;
            if (data[currentIndex] > data[parentIndex])
            {
                SwapElements(currentIndex, parentIndex);
                currentIndex = parentIndex;
            }
            else
            {
                break;
            }
        }
    }

    // Xoa phan tu khoi PriorityQueue "pq.extract();"
    void extract()
    {
        if (count > 0)
        {
            data[0] = data[count - 1];
            count--;

            int currentIndex = 0;
            while (true)
            {
                int leftChildIndex = 2 * currentIndex + 1;
                int rightChildIndex = 2 * currentIndex + 2;
                int largestIndex = currentIndex;

                if (leftChildIndex < count && data[leftChildIndex] > data[largestIndex])
                {
                    largestIndex = leftChildIndex;
                }
                if (rightChildIndex < count && data[rightChildIndex] > data[largestIndex])
                {
                    largestIndex = rightChildIndex;
                }
                if (largestIndex != currentIndex)
                {
                    SwapElements(currentIndex, largestIndex);
                    currentIndex = largestIndex;
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Lay dinh "pq.peek();"
    T peek() const
    {
        if (empty())
            throw runtime_error("Priority Queue is empty!");
        return data[0];
    }

    // Kiem tra rong "pq.empty();"
    bool empty() const
    {
        return count == 0;
    }

    // So luong phan tu trong priority queue "pq.size();"
    int size() const
    {
        return count;
    }

    // Xoa PriorityQueue "pq.clear();"
    void clear()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
        count = 0;
        capacity = 0;
    }
};