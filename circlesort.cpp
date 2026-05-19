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

bool circleSortRecursive(std::vector<int>& a, int low, int high) {
    if (low == high) return false;

    bool swapped = false;
    int left = low, right = high;

    while (left < right) {
        log_compare(left, right);
        log_trace(a);
        if (a[left] > a[right]) {
            swap(a[left], a[right]);
            log_swap(left, right);
            log_trace(a);
            swapped = true;
        }
        ++left;
        --right;
    }
    log_compare(left, right + 1);
    log_trace(a);
    if (left == right && a[left] > a[right + 1]) {
        swap(a[left], a[right + 1]);
        log_swap(left, right + 1);
        log_trace(a);
        swapped = true;
    }

    int mid = (high - low) / 2;
    bool leftSwapped = circleSortRecursive(a, low, low + mid);
    bool rightSwapped = circleSortRecursive(a, low + mid + 1, high);

    return swapped || leftSwapped || rightSwapped;
}
int main() {
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    shuffle(a.begin(), a.end(), rng);

    log_trace(a);
    log_idle();
    
    while(circleSortRecursive(a, 0, N-1));

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