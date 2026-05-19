#include<bits/stdc++.h>
using namespace std;

class Timer {
public:
    Timer() : running(false), elapsed(0) {}

    void start() {
        if (!running) {
            start_time = std::chrono::steady_clock::now();
            running = true;
        }
    }

    void pause() {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
            running = false;
        }
    }

    void reset() {
        running = false;
        elapsed = 0;
    }

    long long getElapsedMilliseconds() const {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            return elapsed + std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        } else {
            return elapsed;
        }
    }

private:
    bool running;
    std::chrono::steady_clock::time_point start_time;
    long long elapsed; // 單位：毫秒
};

vector<int> stableQuickSort(const vector<int>& arr) {
    if (arr.size() <= 1) return arr;

    int pivot = arr[arr.size() / 2];
    vector<int> less, equal, greater;

    for (int x : arr) {
        if (x < pivot) less.push_back(x);
        else if (x == pivot) equal.push_back(x);
        else greater.push_back(x);
    }

    vector<int> sorted;
    vector<int> left = stableQuickSort(less);
    vector<int> right = stableQuickSort(greater);

    sorted.insert(sorted.end(), left.begin(), left.end());
    sorted.insert(sorted.end(), equal.begin(), equal.end());
    sorted.insert(sorted.end(), right.begin(), right.end());

    return sorted;
}

int main() {
    Timer t;
    int n;
    cin >> n;
    vector<int>a(n);
    for (int i = 0 ; i < n ; i++) {
        cin >> a[i];
    }
    t.start();
    a = stableQuickSort(a);
    t.pause();
    for (int i = 0 ; i < n - 1 ; i++) {
        if(a[i] > a[i+1]) {
            cout << "NO\n";
            cout << t.getElapsedMilliseconds() << "ms";
            return 0;
        }
    }
    cout << "Yes\n";
    cout << t.getElapsedMilliseconds() << "ms";
}