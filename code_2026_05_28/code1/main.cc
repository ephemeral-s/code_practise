#include <iostream>
#include <vector>
#include "Sort.h"
#include <ctime>
using namespace std;

void Print(vector<int>& arr) {
    for(auto& e : arr) cout << e << " ";
    cout << endl;
}

int main() {
    srand(time(nullptr));
    vector<int> arr = {6,6,0,2,8,7,7,9,1,4,0,2,3,2,2,9,5,1,4,2};

    HeapSort(arr);
    Print(arr);

    QuickSort(arr, 0, arr.size() - 1);
    Print(arr);

    MergeSort(arr);
    Print(arr);

    return 0;
}