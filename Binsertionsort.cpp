#include<bits/stdc++.h>
using namespace std;

const int N = 128;

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

int bi(const vector<int>& a, int p, int key) {
    int l = 0, r = p;
    while (l < r) {
        int m = (l + r) / 2;
        log_compare(m, p);
        log_trace(a);
        if (a[m] < key)
            l = m + 1;
        else
            r = m;
    }
    return l;
}
int main() {
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    log_trace(a);
    log_idle();

    for (int i = 1; i < N; ++i) {
        int key = a[i];
        int pos = bi(a, i, key);
        for (int j = i; j > pos; --j) {
            log_swap(j, j - 1);
            log_trace(a);
            a[j] = a[j - 1];
        }
        a[pos] = key;
        log_swap(i, pos);
        log_trace(a);
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