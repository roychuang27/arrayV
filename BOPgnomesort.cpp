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

    for (int i = 1; i < n; ++i) {
        // 找出 a[i] 在 a[0..i) 中的插入位置
        auto pos = lower_bound(a.begin(), a.begin() + i, a[i]);

        // 如果已在正確位置，跳過
        if (pos == a.begin() + i) continue;

        // 暫存 a[i]
        auto temp = a[i];

        // 將 pos 到 i-1 的元素往右移一格
        move_backward(pos, a.begin() + i, a.begin() + i + 1);

        // 插入 temp 到 pos
        *pos = temp;
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