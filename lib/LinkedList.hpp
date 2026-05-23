#pragma once
#include <iostream>
using namespace std;

template <typename T>
struct Node
{
    T data;
    Node *next;
};

template <typename T>
class LinkedList
{
    private:
    Node<T> *head;

    public: // <-- Constructor đã được mang ra public
    LinkedList() : head(nullptr) {}
    
    ~LinkedList()
    {
        clear();
    }
    
    Node<T> *createNode(T val)
    {
        Node<T> *newNode = new Node<T>;
        newNode->next = nullptr;
        newNode->data = val;
        return newNode;
    }

    void insertBack(T val)
    {
        if (head == nullptr)
        {
            head = createNode(val);
            return;
        }
        Node<T> *temp = head;

        while (temp->next != nullptr)
            temp = temp->next;
        temp->next = createNode(val);
    }

    void insertFront(T val)
    {
        Node<T> *newNode = createNode(val);
        newNode->next = head;
        head = newNode;
    }

    void insertAt(T val, int index)
    {
        if (index <= 0 || head == nullptr)
        {
            insertFront(val);
            return;
        }
        int k = 0;
        Node<T> *temp = head;
        while (k < index - 1 && temp->next != nullptr)
        {
            temp = temp->next;
            k++;
        }
        Node<T> *newNode = createNode(val);
        newNode->next = temp->next;
        temp->next = newNode;
    }

    void remove(T val)
    {
        if (head == nullptr)
            return;
        if (head->data == val)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
            return;
        }

        Node<T> *pos = head;
        while (pos->next != nullptr && pos->next->data != val)
        {
            pos = pos->next;
        }
        if (pos->next != nullptr)
        {
            Node<T> *temp = pos->next;
            pos->next = temp->next;
            delete temp;
        }
    }

    void removeAt(int index)
    {
        if (head == nullptr)
            return;
        if (index == 0)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
            return;
        }

        int k = 0;
        Node<T> *pos = head;
        while (k < index - 1 && pos->next != nullptr)
        {
            pos = pos->next;
            k++;
        }
        if (pos->next != nullptr)
        {
            Node<T> *temp = pos->next;
            pos->next = temp->next;
            delete temp;
        }
    }

    Node<T> *find(T val) const
    {
        Node<T> *pos = head;
        while (pos != nullptr)
        {
            if (pos->data == val)
                return pos;
            pos = pos->next;
        }
        return nullptr;
    }

    int size() const
    {
        int k = 0;
        Node<T> *temp = head;
        while (temp != nullptr)
        {
            k++;
            temp = temp->next;
        }
        return k;
    }

    void clear()
    {
        if (head == nullptr)
        {
            return;
        }
        while (head != nullptr)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
    }

    void print() const
    {
        if (head == nullptr)
        {
            cout << "List is empty!" << endl;
            return;
        }
        Node<T> *temp = head;
        while (temp != nullptr)
        {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << endl;
    }
};