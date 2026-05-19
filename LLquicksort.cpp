#include<bits/stdc++.h>
using namespace std;

ofstream trace("trace.csv");
ofstream active("active.csv");

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

int N = 256;

int partition(std::vector<int>& a, int low, int high) {
    int pivot = a[high]; 
    int left = low;       // left pointer：追蹤小於 pivot 的區域末端

    for (int i = low; i < high; ++i) {
        log_compare(i, high);
        log_trace(a);
        if (a[i] < pivot) {
            swap(a[i], a[left]);
            log_swap(i, left);
            log_trace(a);
            ++left;
        }
    }
    swap(a[left], a[high]); // 把 pivot 放到正確位置
    log_swap(left, high);
    log_trace(a);
    return left;
}


void quickSort(std::vector<int>& a, int low, int high) {
    if (low < high) {
        int p = partition(a, low, high);
        quickSort(a, low, p - 1);
        quickSort(a, p + 1, high);
    }
}

int main() {
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    quickSort(a, 0, N-1);

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