#include<bits/stdc++.h>
using namespace std;

int N = 96;

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

int main() {
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    bool sorted = false;
    while (!sorted) {
        sorted = true;

        // 偶數階段：比較 (0,1), (2,3), ...
        for (int i = 0; i + 1 < N; i += 2) {
            log_compare(i, i + 1);
            log_trace(a);
            if (a[i] > a[i + 1]) {
                swap(a[i], a[i + 1]);
                log_swap(i, i + 1);
                log_trace(a);
                sorted = false;
            }
        }

        // 奇數階段：比較 (1,2), (3,4), ...
        for (int i = 1; i + 1 < N; i += 2) {
            log_compare(i, i + 1);
            log_trace(a);
            if (a[i] > a[i + 1]) {
                swap(a[i], a[i + 1]);
                log_swap(i, i + 1);
                log_trace(a);
                sorted = false;
            }
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