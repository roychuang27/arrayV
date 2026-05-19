#include <bits/stdc++.h>
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



int N = 512;
int main() {
    int c = 0;
    int s = pow(2, ceil(log2(N)));
    vector<int> a(s);
    iota(a.begin(), a.end(), 1);
    std::mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(a.begin(), a.end(), rng);
    int l1, l2;
    for (int i = 1; i < s; i *= 2) {
        for (int j = 0; j < s; j += i * 2) {
            int v1[i], v2[i];
            for (int k = j; k < j + i; k++) {
                v1[k - j] = a[k];
                log_compare(k, k + i);
                log_trace(a);
            }
            for (int k = j + i; k < j + i * 2; k++){
                v2[k - j - i] = a[k];
            }
            l1 = 0;
            l2 = 0;
            c = 0;
            do {
                if (v1[l1] <= v2[l2] && l1 < i) {
                    a[c + j] = v1[l1];
                    log_swap(c + j, c + j);
                    log_trace(a);
                    l1++;
                } else if (v1[l1] > v2[l2] && l2 == i) {
                    a[c + j] = v1[l1];
                    log_swap(c + j, c + j);
                    log_trace(a);
                    l1++;
                } else {
                    a[c + j] = v2[l2];
                    log_swap(c + j, c + j);
                    log_trace(a);
                    l2++;
                }
                c++;
            } while (l1 < i || l2 < i);
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