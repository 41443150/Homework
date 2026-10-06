\# Homework 1 作業報告



\## 1. 解題說明



\### 問題描述

本次作業包含兩個問題。Problem 1 實作著名的阿克曼函數（Ackermann's function），分別使用遞迴與非遞迴兩種方式求解；Problem 2 則是要求實作遞迴演算法，來列出給定集合的所有可能子集（Powerset）。



\### 解題策略

\- \*\*Problem 1 (Ackermann)\*\*：

&#x20; - 遞迴版：直接依照題目給定的數學定義，使用 `if-else` 判斷式分三種情況呼叫自身即可。

&#x20; - 非遞迴版：因為規定只能使用特定的標頭檔，無法使用內建的 `<stack>`，因此我宣告了一個夠大的整數陣列 `int stack\[100000]` 來模擬推疊，手動控制 `top` 索引值來模擬函式呼叫時的 push 和 pop 動作。

\- \*\*Problem 2 (Powerset)\*\*：

&#x20; - 使用深度優先搜尋 (DFS) 的概念。針對字串中的每一個字元，我們都有兩條路徑可走：「選取該字元」或「不選取該字元」。當走到字串尾端時，就將當前累積的結果印出。



\## 2. 程式實作



```cpp

\#include <algorithm>

\#include <cmath>

\#include <cstdio>

\#include <cstring>

\#include <cstdlib>

\#include <fstream>

\#include <iostream>

\#include <random>

\#include <sstream>

\#include <string>



using namespace std;



// Problem 1: 遞迴版

int ackermann\_recur(int m, int n) {

&#x20;   if (m == 0) return n + 1;

&#x20;   if (m > 0 \&\& n == 0) return ackermann\_recur(m - 1, 1);

&#x20;   return ackermann\_recur(m - 1, ackermann\_recur(m, n - 1));

}



// Problem 1: 非遞迴版

int ackermann\_iter(int m, int n) {

&#x20;   int stack\[100000]; 

&#x20;   int top = 0;

&#x20;   

&#x20;   stack\[top] = m;

&#x20;   stack\[top+1] = n;

&#x20;   top++;



&#x20;   while (top > 0) {

&#x20;       int cur\_n = stack\[top];

&#x20;       top--;

&#x20;       int cur\_m = stack\[top];

&#x20;       top--;



&#x20;       if (cur\_m == 0) {

&#x20;           int ans = cur\_n + 1;

&#x20;           if (top < 0) return ans; 

&#x20;           stack\[top+1] = ans;

&#x20;           top++;

&#x20;       }

&#x20;       else if (cur\_m > 0 \&\& cur\_n == 0) {

&#x20;           stack\[top+1] = cur\_m - 1;

&#x20;           stack\[top+2] = 1;

&#x20;           top += 2;

&#x20;       }

&#x20;       else { 

&#x20;           stack\[top+1] = cur\_m - 1;

&#x20;           stack\[top+2] = cur\_m;

&#x20;           stack\[top+3] = cur\_n - 1;

&#x20;           top += 3;

&#x20;       }

&#x20;   }

&#x20;   return -1; 

}



// Problem 2: Powerset

void powerset(const string\& s, int index, string current) {

&#x20;   if (index == s.length()) {

&#x20;       cout << "(";

&#x20;       for (int i = 0; i < current.length(); i++) {

&#x20;           cout << current\[i];

&#x20;           if (i < current.length() - 1) std::cout << ","; 

&#x20;       }

&#x20;       cout << ") ";

&#x20;       return;

&#x20;   }

&#x20;   

&#x20;   powerset(s, index + 1, current);

&#x20;   powerset(s, index + 1, current + s\[index]);

}



int main() {

&#x20;   cout << "=== Problem 1: Ackermann's function ===" << "\\n";

&#x20;   cout << "Ackermann(3, 2) Recursive: " << ackermann\_recur(3, 2) << "\\n";

&#x20;   cout << "Ackermann(3, 2) Iterative: " << ackermann\_iter(3, 2) << "\\n";

&#x20;   cout << "\\n";



&#x20;   cout << "=== Problem 2: Powerset ===" << "\\n";

&#x20;   string S = "abc";

&#x20;   cout << "Powerset of (a,b,c): \\n";

&#x20;   powerset(S, 0, "");

&#x20;   cout << "\\n";



&#x20;   return 0;

}

```



\## 3. 效能分析



\### Problem 1: Ackermann's function

\- \*\*時間複雜度\*\*：$O(m A(m, n))$。阿克曼函數的成長速度極端快，其執行步驟次數幾乎與回傳的值成正比，時間複雜度在學術上通常以函數本身的運算次數來表示。

\- \*\*空間複雜度\*\*：$O(m)$。在非遞迴版本中，我們使用陣列模擬 Stack，其最大深度（也就是 Stack 同時存在的元素數量）取決於 $m$ 的大小。



\### Problem 2: Powerset

\- \*\*時間複雜度\*\*：$O(2^n)$。對於長度為 $n$ 的集合，每個元素都有選或不選 2 種可能，因此總共會產生 $2^n$ 種子集，且每次都會走到最深處。

\- \*\*空間複雜度\*\*：$O(n)$。遞迴呼叫的最大深度為集合的元素個數 $n$，因此遞迴堆疊需要消耗 $O(n)$ 的空間。



\## 4. 測試與驗證



```shell

$ g++ main.cpp -o main.exe

$ .\\main.exe

=== Problem 1: Ackermann's function ===

Ackermann(3, 2) Recursive: 29

Ackermann(3, 2) Iterative: 29



=== Problem 2: Powerset ===

Powerset of (a,b,c): 

() (c) (b) (b,c) (a) (a,c) (a,b) (a,b,c) 

```



\## 5. 申論及開發報告



這次作業最大的挑戰在於 Problem 1 的非遞迴實作。因為這學期的規範中嚴格限制了可使用的標頭檔，導致無法直接使用 `#include <stack>`。



一開始我卡滿久的，因為想試圖用單純的迴圈來解，後來上網查資料並思考後，發現阿克曼函數這種非線性的遞迴，一定要有類似 Stack 的資料結構才能把前一個狀態「存起來」。因此我決定自己宣告一個夠大的陣列 `int stack\[100000]` 來手動控制 `top` 指標模擬堆疊的操作。撰寫過程中因為陣列的 index 推算錯誤，一直出現無限迴圈或輸出錯誤的值，後來把每次 push 和 pop 的順序畫在紙上推演一次，才成功把非遞迴版寫出來。Powerset 則是利用 DFS 的選與不選概念，相對比較好掌握。

