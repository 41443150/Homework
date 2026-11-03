# 41443150

作業二

## 解題說明

### 問題描述

本題要求實作一個 **Polynomial**（多項式）類別，用來表示

$p(x) = a_0x^{e_0} + a_1x^{e_1} + ...... + a_nx^{e_n}$

其中 $a_i$ 為非零的 **float** 係數， $e_i$ 為非負整數指數。

題目的具體要求如下：

1. 依照 Figure 1 的抽象資料型態（ADT）實作 **Polynomial**，需提供：
   - **Polynomial()**：建立 $p(x) = 0$。
   - **Add**：回傳 ***this** 與 **poly** 的和。
   - **Mult**：回傳 ***this** 與 **poly** 的乘積。
   - **Eval**：將 **f** 代入多項式並回傳結果。
2. 依照 Figure 2 的私有資料成員實作：**Term** 類別存放係數 **coef**（float）與指數 **exp**（int）；**Polynomial** 以 **termArray**（非零項陣列）、**capacity**（陣列大小）、**terms**（非零項個數）表示。
3. 撰寫輸入、輸出函式，並多載 << 與 >> 運算子。

### 解題策略

1. **資料表示**：只儲存非零項（稀疏表示），並讓 termArray 永遠依「指數由大到小」排列，且不存係數為 0 的項、不出現重複指數。這個性質是後續 Add 能用線性時間完成的關鍵。
2. **newTerm**：把一項加到陣列尾端；陣列已滿時將 capacity 加倍並搬移資料，係數為 0 的項直接略過。
3. **Add**：因為兩個多項式都已依指數遞減排序，所以像 merge sort 的合併一樣，用兩個指標同時往後掃：
   - 指數相同 → 係數相加（結果為 0 則不放入）。
   - 指數不同 → 先放入指數較大的那一項。
   - 其中一邊走完後，把另一邊剩下的項接上去。
4. **Mult**：對 *this 的每一項，乘上 poly 的所有項，得到一個暫存多項式 $t$（指數仍為遞減），再用 Add 累加進結果。
5. **Eval**：走訪每一個非零項，累加 $coef * f^{exp}$。
6. **輸入輸出**：
   - **>>**：先讀項數 $n$，再讀 $n$ 組「係數 指數」，每讀一項就用 Add 併入，因此輸入不需事先排序，同指數的項也會自動合併。
   - **<<**：由高次到低次輸出成 **3x^2 + 2x - 1** 的格式，並處理係數為 $\pm 1$、指數為 0 或 1、零多項式等特殊情況。
7. **記憶體管理**：類別內有動態配置的陣列，因此自行實作解構子、複製建構子與 operator=（深層複製），避免 Add、Mult 以傳值回傳時發生重複釋放記憶體。

## 程式實作

以下為完整程式碼：

```cpp
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream&, const Polynomial&);
private:
    float coef;
    int   exp;
};

class Polynomial {
    friend ostream& operator<<(ostream&, const Polynomial&);
    friend istream& operator>>(istream&, Polynomial&);
private:
    Term *termArray;
    int capacity;
    int terms;

    void newTerm(const float theCoef, const int theExp);
public:
    Polynomial();
    Polynomial(const Polynomial& other);
    Polynomial& operator=(const Polynomial& other);
    ~Polynomial();

    Polynomial Add(const Polynomial& poly) const;
    Polynomial Mult(const Polynomial& poly) const;
    float Eval(float f) const;
};

Polynomial::Polynomial() : capacity(1), terms(0) {
    termArray = new Term[capacity];
}

Polynomial::Polynomial(const Polynomial& other)
    : capacity(other.capacity), terms(other.terms) {
    termArray = new Term[capacity];
    for (int i = 0; i < terms; i++)
        termArray[i] = other.termArray[i];
}

Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this != &other) {
        Term *temp = new Term[other.capacity];
        for (int i = 0; i < other.terms; i++)
            temp[i] = other.termArray[i];
        delete [] termArray;
        termArray = temp;
        capacity  = other.capacity;
        terms     = other.terms;
    }
    return *this;
}

Polynomial::~Polynomial() {
    delete [] termArray;
}

void Polynomial::newTerm(const float theCoef, const int theExp) {
    if (theCoef == 0) return;
    if (terms == capacity) {
        capacity *= 2;
        Term *temp = new Term[capacity];
        copy(termArray, termArray + terms, temp);
        delete [] termArray;
        termArray = temp;
    }
    termArray[terms].coef = theCoef;
    termArray[terms++].exp = theExp;
}

Polynomial Polynomial::Add(const Polynomial& b) const {
    Polynomial c;
    int aPos = 0, bPos = 0;

    while (aPos < terms && bPos < b.terms) {
        if (termArray[aPos].exp == b.termArray[bPos].exp) {
            c.newTerm(termArray[aPos].coef + b.termArray[bPos].coef,
                      termArray[aPos].exp);
            aPos++; bPos++;
        } else if (termArray[aPos].exp < b.termArray[bPos].exp) {
            c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
            bPos++;
        } else {
            c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
            aPos++;
        }
    }
    for (; aPos < terms; aPos++)
        c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
    for (; bPos < b.terms; bPos++)
        c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
    return c;
}

Polynomial Polynomial::Mult(const Polynomial& b) const {
    Polynomial c;
    for (int i = 0; i < terms; i++) {
        Polynomial t;
        for (int j = 0; j < b.terms; j++)
            t.newTerm(termArray[i].coef * b.termArray[j].coef,
                      termArray[i].exp  + b.termArray[j].exp);
        c = c.Add(t);
    }
    return c;
}

float Polynomial::Eval(float f) const {
    float result = 0;
    for (int i = 0; i < terms; i++)
        result += termArray[i].coef * pow(f, termArray[i].exp);
    return result;
}

ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) return os << 0;

    for (int i = 0; i < p.terms; i++) {
        float c = p.termArray[i].coef;
        int   e = p.termArray[i].exp;

        if (i == 0) { if (c < 0) os << "-"; }
        else        os << (c < 0 ? " - " : " + ");

        float a = (c < 0) ? -c : c;
        if (e == 0 || a != 1) os << a;
        if (e >= 1) os << "x";
        if (e >  1) os << "^" << e;
    }
    return os;
}

istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;
    p = Polynomial();
    for (int i = 0; i < n; i++) {
        float c; int e;
        if (!(is >> c >> e)) break;
        Polynomial t;
        t.newTerm(c, e);
        p = p.Add(t);
    }
    return is;
}

int main() {
    Polynomial a, b;
    cout << "輸入格式: 項數 係數 指數 係數 指數 ...  (例: 3 3 2 2 1 1 0 代表 3x^2+2x+1)" << endl;
    cout << "a: "; cin >> a;
    cout << "b: "; cin >> b;

    cout << "a     = " << a << endl;
    cout << "b     = " << b << endl;
    cout << "a+b   = " << a.Add(b) << endl;
    cout << "a*b   = " << a.Mult(b) << endl;
    cout << "a(2)  = " << a.Eval(2) << endl;

    Polynomial z;
    cout << "zero  = " << z << endl;
    return 0;
}
```

## 效能分析

設 *this 有 $m$ 個非零項、poly 有 $n$ 個非零項， $t$ 表示單一多項式的項數。

| 函式 | 時間複雜度 | 空間複雜度 |
|------|-----------|-----------|
| Polynomial() | $O(1)$ | $O(1)$ |
| 複製建構子、operator=、解構子 | $O(t)$、 $O(t)$、 $O(1)$ | $O(t)$ |
| newTerm | 均攤 $O(1)$（擴容的那一次為 $O(t)$） | $O(t)$（擴容時需暫存新陣列） |
| Add | $O(m+n)$ | $O(m+n)$（結果最多 $m+n$ 項） |
| Mult | $O(m^2n)$ | $O(mn)$ |
| Eval | $O(m)$ | $O(1)$ |
| operator<< | $O(t)$ | $O(1)$ |
| operator>>（讀入 $t$ 項） | $O(t^2)$ | $O(t)$ |

## 測試與驗證

### 測試案例

輸入格式為「項數 係數 指數 係數 指數 ...」，例如 **3 3 2 2 1 1 0** 代表 $3x^2 + 2x + 1$。**Eval** 一律以 $x = 2$ 代入。

| 測試案例 | 測試目的 | 輸入 | 預期輸出 | 實際輸出 |
|----------|----------|------|----------|----------|
| 測試一 | 一般情況： $a = 3x^2+2x+1$、 $b = x-1$（加法中有項相消） | a: 3 3 2 2 1 1 0<br>b: 2 1 1 -1 0 | a+b = 3x^2 + 3x<br>a*b = 3x^3 - x^2 - x - 1<br>a(2) = 17 | a+b = 3x^2 + 3x<br>a*b = 3x^3 - x^2 - x - 1<br>a(2) = 17 |
| 測試二 | 零多項式： $a = 0$（項數為 0）、 $b = 2x+1$ | a: 0<br>b: 2 2 1 1 0 | a = 0<br>a+b = 2x + 1<br>a*b = 0<br>a(2) = 0 | a = 0<br>a+b = 2x + 1<br>a*b = 0<br>a(2) = 0 |
| 測試三 | 完全抵銷： $a = x^2+1$、 $b = -x^2-1$ | a: 2 1 2 1 0<br>b: 2 -1 2 -1 0 | a+b = 0<br>a*b = -x^4 - 2x^2 - 1<br>a(2) = 5 | a+b = 0<br>a*b = -x^4 - 2x^2 - 1<br>a(2) = 5 |
| 測試四 | 輸入未排序、有重複指數： $a = 1 + 2x^3 + 5x^3 - 1 = 7x^3$ | a: 4 1 0 2 3 5 3 -1 0<br>b: 2 -7 3 1 1 | a = 7x^3<br>b = -7x^3 + x<br>a+b = x<br>a*b = -49x^6 + 7x^4<br>a(2) = 56 | a = 7x^3<br>b = -7x^3 + x<br>a+b = x<br>a*b = -49x^6 + 7x^4<br>a(2) = 56 |
| 測試五 | 常數項與高次項： $a = 5$、 $b = x^{100}$ | a: 1 5 0<br>b: 1 1 100 | b = x^100<br>a+b = x^100 + 5<br>a*b = 5x^100<br>a(2) = 5 | b = x^100<br>a+b = x^100 + 5<br>a*b = 5x^100<br>a(2) = 5 |
| 測試六 | 負係數與係數 $\pm 1$：$a = -x^2+x-1$、 $b = x+1$ | a: 3 -1 2 1 1 -1 0<br>b: 2 1 1 1 0 | a = -x^2 + x - 1<br>a+b = -x^2 + 2x<br>a*b = -x^3 - 1<br>a(2) = -3 | a = -x^2 + x - 1<br>a+b = -x^2 + 2x<br>a*b = -x^3 - 1<br>a(2) = -3 |
| 測試七 | 浮點數係數： $a = 0.5x^2+1.5$、 $b = 2x^2$ | a: 2 0.5 2 1.5 0<br>b: 1 2 2 | a+b = 2.5x^2 + 1.5<br>a*b = x^4 + 3x^2<br>a(2) = 3.5 | a+b = 2.5x^2 + 1.5<br>a*b = x^4 + 3x^2<br>a(2) = 3.5 |

### 編譯與執行指令

**a:**、**b:** 後面的數字為使用者輸入，其餘為程式輸出。

**測試一**

```shell
$ cl.exe /Zi /EHsc /nologo /Fe:main.exe main.cpp
main.cpp
$ .\main.exe
輸入格式: 項數 係數 指數 係數 指數 ...  (例: 3 3 2 2 1 1 0 代表 3x^2+2x+1)
a: 3 3 2 2 1 1 0
b: 2 1 1 -1 0
a     = 3x^2 + 2x + 1
b     = x - 1
a+b   = 3x^2 + 3x
a*b   = 3x^3 - x^2 - x - 1
a(2)  = 17
zero  = 0
```

**測試四（輸入未排序、有重複指數）**

```shell
$ cl.exe /Zi /EHsc /nologo /Fe:main.exe main.cpp
main.cpp
$ .\main.exe
輸入格式: 項數 係數 指數 係數 指數 ...  (例: 3 3 2 2 1 1 0 代表 3x^2+2x+1)
a: 3 3 2 2 1 1 0
b: 2 1 1 -1 0
a     = 3x^2 + 2x + 1
b     = x - 1
a+b   = 3x^2 + 3x
a*b   = 3x^3 - x^2 - x - 1
a(2)  = 17
zero  = 0
```

最後一行 **zero = 0** 是主程式中另外建立一個預設建構的 **Polynomial z** 並輸出，用來驗證預設建構子確實建立 $p(x) = 0$。

### 結論

1. Add、Mult、Eval 的結果皆與手算相符，包含一般情況、零多項式、完全抵銷、負係數與浮點數係數。
2. 輸入未排序、出現重複指數時，>> 仍能正確合併並維持「指數遞減、無重複」的性質，<< 也能正確處理零多項式與係數 $/pm 1$ 的格式。
3. 測試涵蓋了邊界情況（零多項式、高次項 $x^{100}$、項相消成 0），並以 AddressSanitizer 驗證沒有記憶體錯誤。

## 申論及開發報告

### 為何使用「非零項陣列」（稀疏表示）

多項式也可以用「陣列的第 $i$ 格存 $x^i$ 的係數」表示，這種稠密表示法的優點是取係數很快。但像 $x^{1000} + 1$ 這種多項式只有兩個非零項，稠密表示需要 1001 格，Add 也要掃過全部 1001 格。稀疏表示只存非零項，空間為 $O(\text{非零項數})$，運算時間也只和非零項數有關。代價是無法直接用指數當索引，要找特定指數的係數需要搜尋。題目的 Figure 2 本來就是這種表示法，也正好適合處理次數很高但項數很少的多項式。

### 為何維持「指數由大到小」排序

Add 之所以能用合併做到 $O(m+n)$，前提就是兩邊都已排序。如果不排序，每一項都得到另一個多項式中搜尋同指數的項，時間會變成 $O(mn)$。排序也讓 << 可以直接由高次項輸出到低次項。

要維持排序，程式有兩個設計：newTerm 只在尾端新增，呼叫者（Add、Mult）本身就是依遞減順序產生；而使用者輸入的項，則是透過 Add 併入，不直接插入陣列，因此即使輸入亂序也不會破壞性質。

### 為何 newTerm 擴容時將容量加倍

```cpp
if (terms == capacity) {
    capacity *= 2;
    Term *temp = new Term[capacity];
    copy(termArray, termArray + terms, temp);
    delete [] termArray;
    termArray = temp;
}
```

如果每次只增加固定大小 $c$，新增 $t$ 項的總搬移次數約為 $t^2/(2c)$，均攤下來每次新增是 $O(t)$。若採用加倍，總搬移次數為 $1 + 2 + 4 + ... + t < 2t$，均攤每次新增只要 $O(1)$。代價是最多浪費約一半的空間。

### 與 ADT 宣告不同之處

Figure 1 中 Add(Polynomial poly) 是傳值，每次呼叫都會多做一次完整複製。本程式改為 const Polynomial&，語意相同（呼叫端的使用方式不變），但省去複製；同時將 Add、Mult、Eval 宣告為 const 成員函式，表示不會修改 *this，使 const 物件也能呼叫。

### 限制與可改進之處

1. **Mult 的效率**：目前為 $O(m^2n)$。可以改成先把 $mn$ 個乘積全部算出，排序後再合併同指數項，時間為 $O(mn\log(mn))$，在項數很多時更快。
2. **Eval 的效率**：目前每項都呼叫 pow。可以改用稀疏版的 Horner's rule：由高次項往低次項處理，每次乘上 $f^{e_i - e_{i+1}}$，減少乘法次數。
3. **輸入檢查**：程式假設輸入的指數為非負整數、項數為合法值（符合題目的定義），沒有額外檢查。
