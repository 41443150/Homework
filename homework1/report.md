# 41443150

作業一

## 解題說明

### 問題描述
本次作業包含兩個問題。Problem 1 實作著名的阿克曼函數（Ackermann's function），分別使用遞迴與非遞迴兩種方式求解；Problem 2 則是要求實作遞迴演算法，來列出給定集合的所有可能子集（Powerset）。

### 解題策略
- **Problem 1 (Ackermann)**：
  - 遞迴版：直接依照題目給定的數學定義，使用 `if-else` 判斷式分三種情況呼叫自身即可。
  - 非遞迴版：因為規定只能使用特定的標頭檔，無法使用內建的 `<stack>`，因此我宣告了一個夠大的整數陣列 `int stack[100000]` 來模擬推疊，手動控制 `top` 索引值來模擬函式呼叫時的 push 和 pop 動作。
- **Problem 2 (Powerset)**：
  - 使用深度優先搜尋 (DFS) 的概念。針對字串中的每一個字元，我們都有兩條路徑可走：「選取該字元」或「不選取該字元」。當走到字串尾端時，就將當前累積的結果印出。

## 程式實作

```cpp
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

// Problem 1: 遞迴版
int ackermann_recur(int m, int n) {
    if (m == 0) return n + 1;
    if (m > 0 && n == 0) return ackermann_recur(m - 1, 1);
    return ackermann_recur(m - 1, ackermann_recur(m, n - 1));
}

// Problem 1: 非遞迴版
int ackermann_iter(int m, int n) {
    int stack[100000]; 
    int top = 0;
    
    stack[top] = m;
    stack[top+1] = n;
    top++;

    while (top > 0) {
        int cur_n = stack[top];
        top--;
        int cur_m = stack[top];
        top--;

        if (cur_m == 0) {
            int ans = cur_n + 1;
            if (top < 0) return ans; 
            stack[top+1] = ans;
            top++;
        }
        else if (cur_m > 0 && cur_n == 0) {
            stack[top+1] = cur_m - 1;
            stack[top+2] = 1;
            top += 2;
        }
        else { 
            stack[top+1] = cur_m - 1;
            stack[top+2] = cur_m;
            stack[top+3] = cur_n - 1;
            top += 3;
        }
    }
    return -1; 
}

// Problem 2: Powerset
void powerset(const string& s, int index, string current) {
    if (index == s.length()) {
        cout << "(";
        for (int i = 0; i < current.length(); i++) {
            cout << current[i];
            if (i < current.length() - 1) std::cout << ","; 
        }
        cout << ") ";
        return;
    }
    
    powerset(s, index + 1, current);
    powerset(s, index + 1, current + s[index]);
}

int main() {
    cout << "=== Problem 1: Ackermann's function ===" << "\n";
    cout << "Ackermann(3, 2) Recursive: " << ackermann_recur(3, 2) << "\n";
    cout << "Ackermann(3, 2) Iterative: " << ackermann_iter(3, 2) << "\n";
    cout << "\n";

    cout << "=== Problem 2: Powerset ===" << "\n";
    string S = "abc";
    cout << "Powerset of (a,b,c): \n";
    powerset(S, 0, "");
    cout << "\n";

    return 0;
}
```

## 效能分析

### Problem 1: Ackermann's function
- **時間複雜度**：$O(m A(m, n))$。阿克曼函數的成長速度極端快，其執行步驟次數幾乎與回傳的值成正比，時間複雜度在學術上通常以函數本身的運算次數來表示。
- **空間複雜度**：$O(m)$。在非遞迴版本中，我們使用陣列模擬 Stack，其最大深度（也就是 Stack 同時存在的元素數量）取決於 $m$ 的大小。

### Problem 2: Powerset
- **時間複雜度**：$O(2^n)$。對於長度為 $n$ 的集合，每個元素都有選或不選 2 種可能，因此總共會產生 $2^n$ 種子集，且每次都會走到最深處。
- **空間複雜度**：$O(n)$。遞迴呼叫的最大深度為集合的元素個數 $n$，因此遞迴堆疊需要消耗 $O(n)$ 的空間。

## 測試與驗證

```shell
$ g++ main.cpp -o main.exe
$ .\main.exe
=== Problem 1: Ackermann's function ===
Ackermann(3, 2) Recursive: 29
Ackermann(3, 2) Iterative: 29

=== Problem 2: Powerset ===
Powerset of (a,b,c): 
() (c) (b) (b,c) (a) (a,c) (a,b) (a,b,c) 
```

## 申論及開發報告

這次作業最大的挑戰在於 Problem 1 的非遞迴實作。因為這學期的規範中嚴格限制了可使用的標頭檔，導致無法直接使用 `#include <stack>`。

一開始我卡滿久的，因為想試圖用單純的迴圈來解，後來上網查資料並思考後，發現阿克曼函數這種非線性的遞迴，一定要有類似 Stack 的資料結構才能把前一個狀態「存起來」。因此我決定自己宣告一個夠大的陣列 `int stack[100000]` 來手動控制 `top` 指標模擬堆疊的操作。撰寫過程中因為陣列的 index 推算錯誤，一直出現無限迴圈或輸出錯誤的值，後來把每次 push 和 pop 的順序畫在紙上推演一次，才成功把非遞迴版寫出來。Powerset 則是利用 DFS 的選與不選概念，相對比較好掌握。
