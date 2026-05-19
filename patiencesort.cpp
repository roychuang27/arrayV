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

int findPilePosition(const std::vector<std::vector<int>>& piles, int element) {
    int left = 0;
    int right = piles.size();
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (!piles[mid].empty() && piles[mid].back() >= element) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    
    return left;
}

// 建立堆疊（使用迴圈以避免堆疊溢位）
void buildPiles(std::vector<std::vector<int>>& piles, const std::vector<int>& arr) {
    for (int element : arr) {
        int pos = findPilePosition(piles, element);
        
        if (pos == piles.size()) {
            piles.push_back(std::vector<int>());
        }
        
        piles[pos].push_back(element);
    }
}

// 安全的遞迴合併兩個已排序陣列
std::vector<int> mergeTwoArrays(const std::vector<int>& arr1, const std::vector<int>& arr2) {
    std::vector<int> result;
    result.reserve(arr1.size() + arr2.size());
    
    size_t i = 0, j = 0;
    
    // 使用迴圈而非遞迴來避免堆疊溢位
    while (i < arr1.size() && j < arr2.size()) {
        if (arr1[i] <= arr2[j]) {
            result.push_back(arr1[i++]);
        } else {
            result.push_back(arr2[j++]);
        }
    }
    
    while (i < arr1.size()) {
        result.push_back(arr1[i++]);
    }
    
    while (j < arr2.size()) {
        result.push_back(arr2[j++]);
    }
    
    return result;
}

// 遞迴合併多個堆疊（使用分治法）
std::vector<int> mergePilesRecursive(std::vector<std::vector<int>>& piles, int start, int end) {
    // 基本情況：沒有堆疊
    if (start > end) {
        return std::vector<int>();
    }
    
    // 基本情況：只有一個堆疊
    if (start == end) {
        // 反轉堆疊，因為元素是從後面加入的
        std::vector<int> reversed(piles[start].rbegin(), piles[start].rend());
        return reversed;
    }
    
    // 基本情況：只有兩個堆疊（避免過深的遞迴）
    if (end - start == 1) {
        std::vector<int> left(piles[start].rbegin(), piles[start].rend());
        std::vector<int> right(piles[end].rbegin(), piles[end].rend());
        return mergeTwoArrays(left, right);
    }
    
    // 遞迴情況：分治法
    int mid = start + (end - start) / 2;
    
    std::vector<int> leftMerged = mergePilesRecursive(piles, start, mid);
    std::vector<int> rightMerged = mergePilesRecursive(piles, mid + 1, end);
    
    return mergeTwoArrays(leftMerged, rightMerged);
}

// 主要的 Patience Sort 函式
std::vector<int> patienceSort(const std::vector<int>& arr) {
    if (arr.empty()) {
        return std::vector<int>();
    }
    
    if (arr.size() == 1) {
        return arr;
    }
    
    // 建立堆疊
    std::vector<std::vector<int>> piles;
    buildPiles(piles, arr);
    
    // 合併所有堆疊
    if (piles.empty()) {
        return std::vector<int>();
    }
    
    return mergePilesRecursive(piles, 0, piles.size() - 1);
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
    a = patienceSort(a);
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