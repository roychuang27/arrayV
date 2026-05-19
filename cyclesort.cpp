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

void cycleSort(std::vector<int>& a) {
    int n = a.size();
    for (int cycleStart = 0; cycleStart < n - 1; ++cycleStart) {
        int item = a[cycleStart];
        int pos = cycleStart;

        // 找出 item 的正確位置
        for (int i = cycleStart + 1; i < n; ++i)
            if (a[i] < item)
                ++pos;

        // 如果 item 已在正確位置，跳過
        if (pos == cycleStart)
            continue;

        // 跳過重複值
        while (item == a[pos])
            ++pos;

        std::swap(item, a[pos]);

        // 處理整個 cycle
        while (pos != cycleStart) {
            pos = cycleStart;
            for (int i = cycleStart + 1; i < n; ++i)
                if (a[i] < item)
                    ++pos;

            while (item == a[pos])
                ++pos;

            std::swap(item, a[pos]);
        }
    }
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
    cycleSort(a);
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