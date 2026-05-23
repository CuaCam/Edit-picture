#pragma once
#include <iostream>
#include <functional>

// Cau truc Binary Search Tree
template <typename T>
struct BSTNode
{
    T data;
    BSTNode *left;
    BSTNode *right;

    BSTNode(const T &value)
    {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};
template <typename V, typename Comp = std::less<V>>
struct BST
{
private:
    BSTNode<V> *root;
    Comp cmp;
    // Tao node moi
    BSTNode<V> *CreateNode(const V &value)
    {
        return new BSTNode<V>(value);
    }

    // Them node vao cay
    BSTNode<V> *InsertBST(BSTNode<V> *root, const V &value)
    {
        if (root == nullptr)
        {
            return root = CreateNode(value);
        }
        else if (cmp(value, root->data))
        {
            root->left = InsertBST(root->left, value);
        }
        else if (cmp(root->data, value))
        {
            root->right = InsertBST(root->right, value);
        }
        return root;
    }

    // Tim gia tri trong cay
    BSTNode<V> *SearchBST(BSTNode<V> *root, const V &value)
    {
        if (root == nullptr)
        {
            return nullptr;
        }
        else if (cmp(value, root->data))
        {
            return SearchBST(root->left, value);
        }
        else if (cmp(root->data, value))
        {
            return SearchBST(root->right, value);
        }
        return root;
    }

    // Tim gia tri lon nhat ben trai
    BSTNode<V> *findMaxLeft(BSTNode<V> *node) {
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}

    // Xoa node
    BSTNode<V> *RemoveBSTnode(BSTNode<V> *root, const V &value)
    {
        if (root == nullptr)
        {
            return nullptr;
        }
        else if (cmp(value, root->data))
        {
            root->left = RemoveBSTnode(root->left, value);
        }
        else if (cmp(root->data, value))
        {
            root->right = RemoveBSTnode(root->right, value);
        }
        else
        {
            if (root->left == nullptr)
            {
                BSTNode<V> *newRoot = root->right;
                delete root;
                return newRoot;
            }
            else if (root->right == nullptr)
            {
                BSTNode<V> *newRoot = root->left;
                delete root;
                return newRoot;
            }
            else
            {
                BSTNode<V> *newRoot = findMaxLeft(root->left);
                root->data = newRoot->data;
                root->left = RemoveBSTnode(root->left, newRoot->data);
            }
        }
        return root;
    }

    // Traversal
    // Inorder
    void InorderBST(BSTNode<V> *root)
    {
        if (root == nullptr)
            return;
        InorderBST(root->left);
        std::cout << root->data;
        InorderBST(root->right);
    }

    // Preorder
    void PreorderBST(BSTNode<V> *root)
    {
        if (root == nullptr)
            return;
        std::cout << root->data;

        PreorderBST(root->left);
        PreorderBST(root->right);
    }

    // Postorder
    void PostorderBST(BSTNode<V> *root)
    {
        if (root == nullptr)
            return;
        PostorderBST(root->left);
        PostorderBST(root->right);
        std::cout << root->data;
    }

public:
    BST()
    {
        root = nullptr;
    }
    void insert(const V &val)
    {
        root = InsertBST(root, val);
    }
    void remove(const V &val)
    {
        root = RemoveBSTnode(root, val);
    }
    V*search(const V &val)
    {
        BSTNode<V> *node = SearchBST(root, val);
        if (node != nullptr)
        {
            return &(node->data);
        }
        return nullptr;
    }
    void inorder()
    {
        InorderBST(root);
        std::cout << "\n";
    }
    void preorder()
    {
        PreorderBST(root);
        std::cout << "\n";
    }
    void postorder()
    {
        PostorderBST(root);
        std::cout << "\n";
    }
};