#pragma once
#include <iostream>
#include <functional>
using namespace std;

template <typename T>
inline void mySwap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

template <typename T, typename Comp = std::less<T>>
void bubbleSort(T arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (Comp()(arr[j + 1], arr[j]))
            {
                mySwap(arr[j], arr[j + 1]);
            }
        }
    }
}

template <typename T, typename Comp = std::less<T>>
inline void selectionSort(T arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int minIndex = i;
        for (int j = i + 1; j < n; j++)
        {
            if (Comp()(arr[j], arr[minIndex]))
            {
                minIndex = j;
            }
        }
        mySwap(arr[i], arr[minIndex]);
    }
}

template <typename T, typename Comp = std::less<T>>
inline void insertionSort(T arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int j = i;
        while (j > 0 && Comp()(arr[j], arr[j - 1]))
        {
            mySwap(arr[j], arr[j - 1]);
            j--;
        }
    }
}

template <typename T, typename Comp = std::less<T>>
inline void heapify(T arr[], int n, int i)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int extreme = i;

    if (left < n && Comp()(arr[extreme], arr[left]))
        extreme = left;
    if (right < n && Comp()(arr[extreme], arr[right]))
        extreme = right;

    if (extreme != i)
    {
        mySwap(arr[extreme], arr[i]);
        heapify<T, Comp>(arr, n, extreme);
    }
}

template <typename T, typename Comp = std::less<T>>
inline void heapSort(T arr[], int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify<T, Comp>(arr, n, i);
    }

    for (int i = n - 1; i > 0; i--)
    {
        mySwap(arr[0], arr[i]);
        heapify<T, Comp>(arr, i, 0);
    }
}

template <typename T, typename Comp = std::less<T>>
inline int partition(T arr[], int low, int high)
{
    T pivotVal = arr[low + (high - low) / 2];
    int i = low - 1;
    int j = high + 1;
    while (true) 
    {
        do {i++;} while (Comp()(arr[i], pivotVal));
        do {j--;} while (Comp()(pivotVal, arr[j]));
        if (j <= i) return j;

        mySwap(arr[i], arr[j]);
    }
}

template <typename T, typename Comp = std::less<T>>
inline void quickSort(T arr[], int low, int high)
{
    if (low < high)
    {
        int p = partition<T, Comp>(arr, low, high);
        quickSort<T, Comp>(arr, low, p);
        quickSort<T, Comp>(arr, p + 1, high);
    }
}

template <typename T, typename Comp = std::less<T>>
inline void merge(T arr[], int low, int mid, int high)
{
    int n1 = mid - low + 1;
    int n2 = high - mid;
    
    T *L = new T[n1];
    T *R = new T[n2];

    for (int i = 0; i < n1; i++)
    {
        L[i] = arr[i + low];
    }
    for (int i = 0; i < n2; i++)
    {
        R[i] = arr[i + mid + 1];
    }

    int i = 0, j = 0, k = low;
    while (i < n1 && j < n2)
    {
        if (!Comp()(R[j], L[i])) arr[k++] = L[i++];
        else arr[k++] = R[j++];

    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

template <typename T, typename Comp = std::less<T>>
inline void mergeSort(T arr[], int low, int high)
{
    if (low < high) 
    {
        int mid = low + (high - low)/2;
        mergeSort<T, Comp>(arr, low, mid);
        mergeSort<T, Comp>(arr, mid + 1, high);
        merge<T, Comp>(arr, low, mid, high);
    }
}

template <typename T>
inline int linearSort(T arr[], int n, T key)
{
    for (int i = 0; i < n; i++)
    {
        if (arr[i] == key)
            return i;
    }
    return -1;
}

template <typename T>
inline int binarySearch(T arr[], int n, T key)
{
    int low = 0, high = n - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        if (arr[mid] == key)
            return mid;
        else if (arr[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}