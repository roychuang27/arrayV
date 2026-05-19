#include <bits/stdc++.h>
using namespace std;

// 🔧 Logging 全域變數
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


int partition(vector<int>& a, int low, int high) {
    int pivot = a[low];
    int left = low - 1;
    int right = high + 1;

    while (true) {
        do {
            ++left;
            log_compare(left, low);
            log_trace(a);

        } while (a[left] < pivot);

        do {
            --right;
            log_compare(right, low);
            log_trace(a);

        } while (a[right] > pivot);

        if (left >= right) return right;

        swap(a[left], a[right]);
        log_swap(left, right);
        log_trace(a);

    }
}

void quickSort(vector<int>& a, int low, int high) {
    if (low < high) {
        int p = partition(a, low, high);
        quickSort(a, low, p);
        quickSort(a, p + 1, high);
    }
}
int N = 256;
int main() {
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    quickSort(a, 0, N-1);
    // 標記所有已排序元素
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