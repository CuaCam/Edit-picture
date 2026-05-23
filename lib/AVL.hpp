#pragma once
#include <functional>
#include <iostream>

template <typename T, typename Comp = std::less<T>>
struct AVL
{
private:
    struct AVLNode
    {
        T data;
        AVLNode *left;
        AVLNode *right;
        int height;

        AVLNode(const T &val)
        {
            data = val;
            left = nullptr;
            right = nullptr;
            height = 1;
        }
    };

    AVLNode *root;
    Comp cmp;

    int getHeight(AVLNode *p)
    {
        if (p == nullptr)
            return 0;
        return p->height;
    }

    void updateHeight(AVLNode *p)
    {
        if (p == nullptr)
            return;
        int hl = getHeight(p->left);
        int hr = getHeight(p->right);
        p->height = (hl > hr ? hl : hr) + 1;
    }

    int balanceFactor(AVLNode *p)
    {
        if (p == nullptr)
            return 0;
        return getHeight(p->right) - getHeight(p->left);
    }

    AVLNode *rightRotation(AVLNode *p)
    {
        AVLNode *newParent = p->left;
        AVLNode *orphan = newParent->right;
        newParent->right = p;
        p->left = orphan;
        updateHeight(p);
        updateHeight(newParent);
        return newParent;
    }

    AVLNode *leftRotation(AVLNode *p)
    {
        AVLNode *newParent = p->right;
        AVLNode *orphan = newParent->left;
        newParent->left = p;
        p->right = orphan;
        updateHeight(p);
        updateHeight(newParent);
        return newParent;
    }

    AVLNode *lrRotation(AVLNode *p)
    {
        p->left = leftRotation(p->left);
        return rightRotation(p);
    }

    AVLNode *rlRotation(AVLNode *p)
    {
        p->right = rightRotation(p->right);
        return leftRotation(p);
    }

    AVLNode *InsertAVL(AVLNode *p, const T &val)
    {
        if (p == nullptr)
            return new AVLNode(val); // Đã sửa lỗi thêm <T> thừa
        if (cmp(val, p->data))
            p->left = InsertAVL(p->left, val);
        else if (cmp(p->data, val))
            p->right = InsertAVL(p->right, val);
        else
            return p;

        updateHeight(p);
        int bf = balanceFactor(p);

        if (bf < -1)
        {
            if (balanceFactor(p->left) <= 0)
                return rightRotation(p);
            else
                return lrRotation(p);
        }
        else if (bf > 1)
        {
            if (balanceFactor(p->right) >= 0)
                return leftRotation(p);
            else
                return rlRotation(p);
        }
        return p;
    }

    AVLNode *minNode(AVLNode *p)
    {
        while (p->left != nullptr)
            p = p->left;
        return p;
    }

    AVLNode *RemoveAVL(AVLNode *p, const T &val)
    {
        if (p == nullptr)
            return nullptr;
        if (cmp(val, p->data))
            p->left = RemoveAVL(p->left, val);
        else if (cmp(p->data, val))
            p->right = RemoveAVL(p->right, val);
        else
        {
            if (!p->left || !p->right)
            {
                AVLNode *child = p->left ? p->left : p->right;
                delete p;
                return child;
            }
            AVLNode *successor = minNode(p->right);
            p->data = successor->data;
            p->right = RemoveAVL(p->right, successor->data);
        }

        updateHeight(p);
        int bf = balanceFactor(p);

        if (bf < -1)
        {
            if (balanceFactor(p->left) <= 0)
                return rightRotation(p);
            else
                return lrRotation(p);
        }
        else if (bf > 1)
        {
            if (balanceFactor(p->right) >= 0)
                return leftRotation(p);
            else
                return rlRotation(p);
        }
        return p;
    }

    AVLNode *SearchAVL(AVLNode *p, const T &val)
    {
        if (p == nullptr)
            return nullptr;
        if (!cmp(val, p->data) && !cmp(p->data, val))
            return p;
        if (cmp(val, p->data))
            return SearchAVL(p->left, val);
        return SearchAVL(p->right, val);
    }

    void inorderAVL(AVLNode *p)
    {
        if (p == nullptr)
            return;
        inorderAVL(p->left);
        std::cout << p->data << " ";
        inorderAVL(p->right);
    }

    void preorderAVL(AVLNode *p)
    {
        if (p == nullptr)
            return;
        std::cout << p->data << " ";
        preorderAVL(p->left);
        preorderAVL(p->right);
    }

    void postorderAVL(AVLNode *p)
    {
        if (p == nullptr)
            return;
        postorderAVL(p->left);
        postorderAVL(p->right);
        std::cout << p->data << " ";
    }

    void clearTree(AVLNode *p) // Đã sửa tên biến thành p
    {
        if (p == nullptr)
            return;
        clearTree(p->left);
        clearTree(p->right);
        delete p;
    }

public:
    AVL()
    {
        root = nullptr;
    }

    void insert(const T &val)
    {
        root = InsertAVL(root, val);
    }

    void remove(const T &val)
    {
        root = RemoveAVL(root, val);
    }

    T *search(const T &val)
    {
        AVLNode *node = SearchAVL(root, val);
        if (node != nullptr)
        {
            return &(node->data);
        }
        return nullptr;
    }
    void inorder()
    {
        inorderAVL(root);
        std::cout << "\n";
    }
    void preorder()
    {
        preorderAVL(root);
        std::cout << "\n";
    }
    void postorder()
    {
        postorderAVL(root);
        std::cout << "\n";
    }

    void clear()
    {
        clearTree(root);
        root = nullptr;
    }
    
    ~AVL()
    {
        clear();
    }
};