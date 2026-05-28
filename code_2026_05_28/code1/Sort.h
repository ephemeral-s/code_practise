#pragma once

#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

void AdjustUp(vector<int>& arr, int child) {
    int parent = (child - 1) / 2;
    while(child > 0) {
        if(arr[child] > arr[parent]) {
            swap(arr[child], arr[parent]);
            child = parent;
            parent = (parent - 1) / 2;
        }
        else {
            break;
        }
    }
}

void AdjustDown(vector<int>& arr, int parent, int size) {
    int child = parent * 2 + 1;
    while(child < size) {
        if(child + 1 < size && arr[child] < arr[child + 1])  child++;
        if(arr[parent] < arr[child]) {
            swap(arr[parent], arr[child]);
            parent = child;
            child = child * 2 + 1;
        }
        else break;
    }
}

void HeapSort(vector<int>& arr) {
    //从左到右遍历，向上调整建大堆
    for(int i = 0; i < arr.size(); i++) {
        AdjustUp(arr, i);
    }

    //每次将堆顶元素与末尾元素交换，然后向下调整末尾元素
    for(int i = arr.size() - 1;  i >= 0; i--) {
        swap(arr[i], arr[0]);
        AdjustDown(arr, 0, i);
    }
}

// int _QuickSort(vector<int>& arr, int left, int right) {
//     int key = left++;
//     while(left <= right) {
//         while(left <= right && arr[left] < arr[key]) left++;
//         while(left <= right && arr[right] > arr[key]) right--;
//         if(left <= right) swap(arr[left++], arr[right--]);
//     }
//     swap(arr[right], arr[key]);
//     return right;
// }

// int _QuickSort(vector<int>& arr, int left, int right) {
//     int hole = left, key = arr[hole];
//     while(left < right) {
//         while(left < right && arr[right] >= key) right--;
//         arr[hole] = arr[right];
//         hole = right;

//         while(left < right && arr[left] <= key) left++;
//         arr[hole] = arr[left];
//         hole = left;
//     }
//     arr[hole] = key;
//     return hole;
// }

// void QuickSort(vector<int>& arr, int left, int right) {
//     if(left >= right) return;
//     int key = _QuickSort(arr, left, right);

//     QuickSort(arr, left, key - 1);
//     QuickSort(arr, key + 1, right);
// }

void QuickSort(vector<int>& arr, int left, int right) {
    if(left >= right) return;
    int begin = left, end = right, cur = left;
    int key = arr[rand() % (right - left + 1) + left];
    while(cur <= right) {
        if(arr[cur] < key) swap(arr[cur++], arr[left++]);
        else if(arr[cur] > key) swap(arr[cur], arr[right--]);
        else cur++;
    }
    QuickSort(arr, begin, left - 1);
    QuickSort(arr, right + 1, end);
}

void _MergeSort(vector<int>& arr, int left, int right, vector<int>& tmp) {
    if(left >= right) return;
    int mid = left + (right - left) / 2;
    _MergeSort(arr, left, mid, tmp);
    _MergeSort(arr, mid + 1, right, tmp);

    int begin1 = left, end1 = mid;
    int begin2 = mid + 1, end2 = right;
    int index = left;
    while(begin1 <= end1 && begin2 <= end2) 
        if(arr[begin1] <= arr[begin2]) tmp[index++] = arr[begin1++];
        else tmp[index++] = arr[begin2++];
    while(begin1 <= end1) tmp[index++] = arr[begin1++];
    while(begin2 <= end2) tmp[index++] = arr[begin2++];
    for(int i = left; i <= right; i++) arr[i] = tmp[i];
}

void MergeSort(vector<int>& arr) {
    vector<int> tmp(arr);
    _MergeSort(arr, 0, arr.size() - 1, tmp);
}