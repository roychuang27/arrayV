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
int main() {
    Timer t;
    int n;
    cin >> n;
    vector<int>a(n);
    for (int i = 0 ; i < n ; i++) {
        cin >> a[i];
    }
    t.start();
    for (int i = 0 ; i < n && t.getElapsedMilliseconds() < 10000 ; i++) {
        bool swapped = false;
        for (int j = 0 ; j < n - 1 - i ; j++) {
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
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