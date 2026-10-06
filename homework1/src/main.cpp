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

// Problem 1: 遞迴版 Ackermann function
int ackermann_recur(int m, int n) {
    if (m == 0) return n + 1;
    if (m > 0 && n == 0) return ackermann_recur(m - 1, 1);
    return ackermann_recur(m - 1, ackermann_recur(m, n - 1));
}

// Problem 1: 非遞迴版 Ackermann function
// 因為不能 include <stack>，所以用陣列做一個簡單的 stack
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
        // cout << "debug: m=" << cur_m << ", n=" << cur_n << endl;

        if (cur_m == 0) {
            int ans = cur_n + 1;
            if (top < 0) return ans; // stack 空了就代表算完了
            
            stack[top+1] = ans;
            top++;
        }
        else if (cur_m > 0 && cur_n == 0) {
            stack[top+1] = cur_m - 1;
            stack[top+2] = 1;
            top += 2;
        }
        else { 
            // m > 0 且 n > 0
            stack[top+1] = cur_m - 1;
            stack[top+2] = cur_m;
            stack[top+3] = cur_n - 1;
            top += 3;
        }
    }
    return -1; 
}

// Problem 2: 遞迴版 Powerset
// 用字串來紀錄目前的子集狀態
void powerset(const string& s, int index, string current) {
    // 走到最後一個元素了，印出目前的組合
    if (index == s.length()) {
        cout << "(";
        for (int i = 0; i < current.length(); i++) {
            cout << current[i];
            if (i < current.length() - 1) cout << ",";
        }
        cout << ") ";
        return;
    }
    
    // 分支 1: 不要選現在這個字元
    powerset(s, index + 1, current);
    
    // 分支 2: 要選現在這個字元
    powerset(s, index + 1, current + s[index]);
}

int main() {
    cout << "Problem 1: Ackermann's function" << "\n";
    cout << "Ackermann(3, 2) Recursive: " << ackermann_recur(3, 2) << "\n";
    cout << "Ackermann(3, 2) Iterative: " << ackermann_iter(3, 2) << "\n";
    cout << "\n";

    cout << "Problem 2: Powerset" << "\n";
    string S = "abc";
    cout << "Powerset of (a,b,c): \n";
    powerset(S, 0, "");
    cout << "\n";

    return 0;
}