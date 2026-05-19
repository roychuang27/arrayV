#include<bits/stdc++.h>
using namespace std;

ofstream trace("trace.csv");
ofstream active("active.csv");

int N = 256;

void log_trace(const vector<int>& a) {
    for (int i = 0; i < a.size(); ++i)
        trace << a[i] << (i + 1 == a.size() ? "\n" : ",");
}

void log_swap(int i, int j) {
    active << "swap," << i << "," << j << "\n";
}

void log_compare(int i, int j) {
    active << "compare," << i << "," << j << "\n";
}

void log_sorted(const vector<int>& indices) {
    active << "sorted";
    for (int idx : indices)
        active << "," << idx;
    active << "\n";
}

void log_idle() {
    active << "idle\n";
}

void heapify(std::vector<int>& a, int n, int i) {
    int largest = i;          // 假設根節點最大
    int left = 2 * i + 1;     // 左子節點
    int right = 2 * i + 2;    // 右子節點

    // 如果左子節點存在且比根節點大
    if (left < n && a[left] > a[largest]) {
        largest = left;
        log_compare(left, largest);
        log_trace(a);
    }
    // 如果右子節點存在且比目前最大值還大
    if (right < n && a[right] > a[largest]) {
        largest = right;
        log_compare(right, largest);
        log_trace(a);
    }
    log_compare(i, largest);
    log_trace(a);
    // 如果最大值不是根節點，交換並遞迴 heapify
    if (largest != i) {
        swap(a[i], a[largest]);
        log_swap(i, largest);
        log_trace(a);
        heapify(a, n, largest);
    }
}

// 主排序函式
void heapSort(std::vector<int>& a) {
    int n = a.size();

    // Step 1️：建堆（Build Max Heap）
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(a, n, i);

    // Step 2️：排序（Extract 最大值）
    for (int i = n - 1; i > 0; --i) {
        swap(a[0], a[i]); // 最大值移到尾端
        log_swap(0, i);
        log_trace(a);     
        heapify(a, i, 0);          // 重建堆（不含尾端）
    }
}


int main() {
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    heapSort(a);

    vector<int> sorted_indices(N);
    for (int i = 0 ; i < N - 1 ; i++) {
        log_trace(a);
        log_compare(i, i+1);
    } 
    iota(sorted_indices.begin(), sorted_indices.end(), 0);
    log_trace(a);
    log_sorted(sorted_indices);

    trace.close();
    active.close();
    return 0;
}