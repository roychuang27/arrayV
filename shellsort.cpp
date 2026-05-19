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

int main() {
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    for (int gap = N / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < N; ++i) {
            int key = a[i];
            int j = i;
            // 對 gap 間距的子序列做插入排序
            while (j >= gap && a[j - gap] > key) {
                log_compare(j - gap, i);
                log_trace(a);
                a[j] = a[j - gap];
                log_swap(j - gap, j);
                log_trace(a);
                j -= gap;
            }
            a[j] = key;
            log_swap(j, i);
            log_trace(a);
        }
    }

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