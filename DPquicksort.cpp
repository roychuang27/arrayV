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

void dualPivotQuickSort(vector<int>& arr, int low, int high) {
    if (low >= high) return;

    // 選擇兩個 pivot
    log_compare(low, high);
    log_trace(arr);
    if (arr[low] > arr[high]) {
        swap(arr[low], arr[high]);
        log_swap(low, high);
        log_trace(arr);
    }
    int pivot1 = arr[low];
    int pivot2 = arr[high];

    int lt = low + 1;
    int gt = high - 1;
    int i = lt;

    while (i <= gt) {
        log_compare(i, pivot1);
        log_trace(arr);
        log_compare(i, pivot2);
        log_trace(arr);
        if (arr[i] < pivot1) {
            swap(arr[i], arr[lt]);
            log_swap(i, lt);
            log_trace(arr);
            lt++;
        }
        else if (arr[i] > pivot2) {
            while (arr[gt] > pivot2 && i < gt) {
                gt--;
            }
            swap(arr[i], arr[gt]);
            log_swap(i, gt);
            log_trace(arr);
            gt--;
            log_compare(i, pivot1);
            log_trace(arr);
            if (arr[i] < pivot1) {
                swap(arr[i], arr[lt]);
                log_swap(i, lt);
                log_trace(arr);
                lt++;
            }
        }
        i++;
    }

    lt--; gt++;
    swap(arr[low], arr[lt]);
    swap(arr[high], arr[gt]);
    log_swap(low, lt);
    log_trace(arr);
    log_swap(high, gt);
    log_trace(arr);
    // 遞迴排序三個區段
    dualPivotQuickSort(arr, low, lt - 1);
    dualPivotQuickSort(arr, lt + 1, gt - 1);
    dualPivotQuickSort(arr, gt + 1, high);
}

int main() {
    ios::sync_with_stdio(false);
    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);

    dualPivotQuickSort(a, 0, N-1);

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