#include<bits/stdc++.h>
using namespace std;

ofstream trace("trace.csv");
ofstream active("active.csv");

int N = 96;

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
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    shuffle(a.begin(), a.end(), rng);

    log_trace(a);
    log_idle();

    for (int i = 0 ; i < N / 2 ; i++) {
        int mi = INT_MAX, ma = -1;
        int p1 = 0, p2 = N - 1;
        for (int j = i ; j < N - i ; j++) {
            log_compare(j, p1);
            log_trace(a);
            if (a[j] < mi) {
                p1 = j;
                mi = a[j];
            }
            log_compare(j, p2);
            log_trace(a);
            if (a[j] > ma) {
                p2 = j;
                ma = a[j];
            }
        }
        swap(a[i], a[p1]);
        log_swap(i, p1);
        log_trace(a);
        if (p2 == i) p2 = p1;
        swap(a[N - i - 1], a[p2]);
        log_swap(N - i - 1, p2);
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