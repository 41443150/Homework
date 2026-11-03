# （填入學號）

作業二

## 解題說明

### 問題描述

本題要求實作一個 **Polynomial**（多項式）類別，用來表示

$p(x) = a_0x^{e_0} + a_1x^{e_1} + ...... + a_nx^{e_n}$

其中 $a_i$ 為非零的 **float** 係數，$e_i$ 為非負整數指數。

題目的具體要求如下：

1. 依照 Figure 1 的抽象資料型態（ADT）實作 `Polynomial`，需提供：
   - `Polynomial()`：建立 $p(x) = 0$。
   - `Add`：回傳 `*this` 與 `poly` 的和。
   - `Mult`：回傳 `*this` 與 `poly` 的乘積。
   - `Eval`：將 `f` 代入多項式並回傳結果。
2. 依照 Figure 2 的私有資料成員實作：`Term` 類別存放係數 `coef`（`float`）與指數 `exp`（`int`）；`Polynomial` 以 `termArray`（非零項陣列）、`capacity`（陣列大小）、`terms`（非零項個數）表示。
3. 撰寫輸入、輸出函式，並多載 `<<` 與 `>>` 運算子。

### 解題策略

1. **資料表示**：只儲存非零項（稀疏表示），並讓 `termArray` 永遠依「指數由大到小」排列，且不存係數為 0 的項、不出現重複指數。這個性質是後續 `Add` 能用線性時間完成的關鍵。
2. **`newTerm`**：把一項加到陣列尾端；陣列已滿時將 `capacity` 加倍並搬移資料，係數為 0 的項直接略過。
3. **`Add`**：因為兩個多項式都已依指數遞減排序，所以像 merge sort 的合併一樣，用兩個指標同時往後掃：
   - 指數相同 → 係數相加（結果為 0 則不放入）。
   - 指數不同 → 先放入指數較大的那一項。
   - 其中一邊走完後，把另一邊剩下的項接上去。
4. **`Mult`**：對 `*this` 的每一項，乘上 `poly` 的所有項，得到一個暫存多項式 $t$（指數仍為遞減），再用 `Add` 累加進結果。
5. **`Eval`**：走訪每一個非零項，累加 $coef \times f^{exp}$。
6. **輸入輸出**：
   - `>>`：先讀項數 $n$，再讀 $n$ 組「係數 指數」，每讀一項就用 `Add` 併入，因此輸入不需事先排序，同指數的項也會自動合併。
   - `<<`：由高次到低次輸出成 `3x^2 + 2x - 1` 的格式，並處理係數為 $\pm 1$、指數為 0 或 1、零多項式等特殊情況。
7. **記憶體管理**：類別內有動態配置的陣列，因此自行實作解構子、複製建構子與 `operator=`（深層複製），避免 `Add`、`Mult` 以傳值回傳時發生重複釋放記憶體。

### 範例說明

**`Add` 範例**：設 $a = 3x^2 + 2x + 1$、$b = x - 1$。

| 步驟 | $a$ 目前項 | $b$ 目前項 | 動作 | 結果陣列 |
|------|-----------|-----------|------|----------|
| 1 | $3x^2$ | $x$ | $a$ 的指數較大，放入 $3x^2$ | $3x^2$ |
| 2 | $2x$ | $x$ | 指數相同，係數相加 $2+1=3$ | $3x^2,\ 3x$ |
| 3 | $1$ | $-1$ | 指數相同，$1+(-1)=0$，`newTerm` 略過 | $3x^2,\ 3x$ |
| 4 | 結束 | 結束 | 迴圈結束 | $3x^2 + 3x$ |

**`Mult` 範例**：同樣的 $a$、$b$，逐項相乘後累加：

1. 取 $a$ 的 $3x^2$：$t = 3x^3 - 3x^2$，累加後 $c = 3x^3 - 3x^2$。
2. 取 $a$ 的 $2x$：$t = 2x^2 - 2x$，累加後 $c = 3x^3 - x^2 - 2x$。
3. 取 $a$ 的 $1$：$t = x - 1$，累加後 $c = 3x^3 - x^2 - x - 1$。

## 程式實作

以下為完整程式碼：

```cpp
/*
 * DS-1 Homework 2 : Polynomial class
 *
 * 以「非零項陣列」表示多項式 (稀疏表示法)。
 * 重要性質 (invariant)：termArray 中的項永遠依「指數由大到小」排列，
 *                      且不會存放係數為 0 的項，也不會有指數重複的項。
 * 這個性質讓 Add 可以用「合併 (merge)」的方式在線性時間內完成。
 */
#include <iostream>
#include <cmath>      // pow
#include <algorithm>  // copy
using namespace std;

class Polynomial;     // forward declaration，因為 Term 內要用到 Polynomial

/* ---------------- Term：多項式中的一項 (係數 coef, 指數 exp) ---------------- */
class Term {
    friend class Polynomial;                              // 讓 Polynomial 能存取 private 成員
    friend ostream& operator<<(ostream&, const Polynomial&); // friend 不具傳遞性，輸出函式也要單獨宣告
private:
    float coef;   // 係數
    int   exp;    // 指數 (非負整數)
};

/* ---------------- Polynomial ---------------- */
class Polynomial {
    // 輸入/輸出運算子需要存取 private 成員，所以宣告為 friend
    friend ostream& operator<<(ostream&, const Polynomial&);
    friend istream& operator>>(istream&, Polynomial&);
private:
    Term *termArray;  // 動態陣列，存放非零項
    int capacity;     // termArray 的大小 (最多可放幾項)
    int terms;        // 目前實際有幾個非零項

    // 在陣列尾端新增一項；呼叫者需保證指數比現有各項都小 (維持遞減順序)
    void newTerm(const float theCoef, const int theExp);
public:
    Polynomial();                                     // 建立 p(x) = 0
    Polynomial(const Polynomial& other);              // 複製建構子 (深層複製)
    Polynomial& operator=(const Polynomial& other);   // 指定運算子 (深層複製)
    ~Polynomial();                                    // 釋放動態記憶體

    Polynomial Add(const Polynomial& poly) const;     // 回傳 *this + poly
    Polynomial Mult(const Polynomial& poly) const;    // 回傳 *this * poly
    float Eval(float f) const;                        // 回傳 p(f)
};

/* 建構子：建立零多項式 (沒有任何項)，初始容量為 1 */
Polynomial::Polynomial() : capacity(1), terms(0) {
    termArray = new Term[capacity];
}

/* 複製建構子：
 * 若不自己寫，編譯器只會複製 termArray 這個「指標」，
 * 兩個物件的解構子會 delete[] 同一塊記憶體 (double free)。
 * 所以必須配置新的陣列並逐項複製。 */
Polynomial::Polynomial(const Polynomial& other)
    : capacity(other.capacity), terms(other.terms) {
    termArray = new Term[capacity];
    for (int i = 0; i < terms; i++)
        termArray[i] = other.termArray[i];
}

/* 指定運算子：先配置並複製新資料，成功後才釋放舊資料。
 * this != &other 是為了處理自己指定給自己 (a = a) 的情況。 */
Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this != &other) {
        Term *temp = new Term[other.capacity];
        for (int i = 0; i < other.terms; i++)
            temp[i] = other.termArray[i];
        delete [] termArray;            // 釋放舊陣列
        termArray = temp;
        capacity  = other.capacity;
        terms     = other.terms;
    }
    return *this;
}

/* 解構子：釋放動態配置的陣列 */
Polynomial::~Polynomial() {
    delete [] termArray;
}

/* newTerm：新增一項到陣列尾端。
 * - 係數為 0 的項不存 (維持「只存非零項」)
 * - 陣列已滿時，容量加倍再搬移資料 (平均每次新增為 O(1)) */
void Polynomial::newTerm(const float theCoef, const int theExp) {
    if (theCoef == 0) return;
    if (terms == capacity) {
        capacity *= 2;
        Term *temp = new Term[capacity];
        copy(termArray, termArray + terms, temp);   // 搬移舊資料
        delete [] termArray;
        termArray = temp;
    }
    termArray[terms].coef = theCoef;
    termArray[terms++].exp = theExp;
}

/* Add：兩個遞減排序的陣列做合併 (類似 merge sort 的 merge)。
 * aPos / bPos 分別指向 *this 與 b 目前處理的項。
 * 時間複雜度 O(m + n)。 */
Polynomial Polynomial::Add(const Polynomial& b) const {
    Polynomial c;                       // 存放結果
    int aPos = 0, bPos = 0;

    while (aPos < terms && bPos < b.terms) {
        if (termArray[aPos].exp == b.termArray[bPos].exp) {
            // 指數相同 → 係數相加 (和為 0 時 newTerm 會自動略過)
            c.newTerm(termArray[aPos].coef + b.termArray[bPos].coef,
                      termArray[aPos].exp);
            aPos++; bPos++;
        } else if (termArray[aPos].exp < b.termArray[bPos].exp) {
            // b 的指數較大 → 先放 b 的項
            c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
            bPos++;
        } else {
            // a 的指數較大 → 先放 a 的項
            c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
            aPos++;
        }
    }
    // 其中一個多項式處理完後，把另一個剩下的項直接接上去
    for (; aPos < terms; aPos++)
        c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
    for (; bPos < b.terms; bPos++)
        c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
    return c;
}

/* Mult：*this 的每一項 × b 的所有項，產生一個暫存多項式 t，
 * 再用 Add 累加到結果 c。
 * t 的指數仍為遞減 (b 遞減，每項都加上同一個指數)，所以可以直接用 newTerm。
 * 最壞時間複雜度約 O(m^2 * n)，m、n 為兩多項式的項數。 */
Polynomial Polynomial::Mult(const Polynomial& b) const {
    Polynomial c;
    for (int i = 0; i < terms; i++) {
        Polynomial t;
        for (int j = 0; j < b.terms; j++)
            t.newTerm(termArray[i].coef * b.termArray[j].coef,   // 係數相乘
                      termArray[i].exp  + b.termArray[j].exp);   // 指數相加
        c = c.Add(t);       // 把這一輪的結果合併進 c (同指數項會自動相加)
    }
    return c;
}

/* Eval：計算 p(f) = Σ coef * f^exp，只需走訪非零項，時間 O(terms)。 */
float Polynomial::Eval(float f) const {
    float result = 0;
    for (int i = 0; i < terms; i++)
        result += termArray[i].coef * pow(f, termArray[i].exp);
    return result;
}

/* 輸出運算子 <<：輸出成類似 3x^2 + 2x - 1 的格式。
 * 特殊情況處理：零多項式輸出 0；係數為 ±1 時省略 1；
 *              指數為 1 時不印 ^1；指數為 0 時只印係數。 */
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) return os << 0;               // 零多項式

    for (int i = 0; i < p.terms; i++) {
        float c = p.termArray[i].coef;
        int   e = p.termArray[i].exp;

        // 符號：第一項只有負號才印；之後的項用 " + " 或 " - " 連接
        if (i == 0) { if (c < 0) os << "-"; }
        else        os << (c < 0 ? " - " : " + ");

        float a = (c < 0) ? -c : c;                 // 係數取絕對值 (符號已印過)
        if (e == 0 || a != 1) os << a;              // 常數項一定要印；係數為 1 的 x 項省略
        if (e >= 1) os << "x";
        if (e >  1) os << "^" << e;
    }
    return os;
}

/* 輸入運算子 >>：格式為「項數 n，接著 n 組 係數 指數」。
 * 例：3 3 2 2 1 1 0 代表 3x^2 + 2x + 1。
 * 每讀到一項就用 Add 併入 p，因此即使輸入沒排序、
 * 有重複指數也能維持「遞減、無重複」的性質。 */
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;                      // 讀取失敗就直接返回
    p = Polynomial();                               // 先清成零多項式
    for (int i = 0; i < n; i++) {
        float c; int e;
        if (!(is >> c >> e)) break;
        Polynomial t;                               // 只含這一項的多項式
        t.newTerm(c, e);
        p = p.Add(t);
    }
    return is;
}

/* ---------------- 測試用主程式 ---------------- */
int main() {
    Polynomial a, b;
    cout << "輸入格式: 項數 係數 指數 係數 指數 ...  (例: 3 3 2 2 1 1 0 代表 3x^2+2x+1)" << endl;
    cout << "a: "; cin >> a;
    cout << "b: "; cin >> b;

    cout << "a     = " << a << endl;
    cout << "b     = " << b << endl;
    cout << "a+b   = " << a.Add(b) << endl;
    cout << "a*b   = " << a.Mult(b) << endl;
    cout << "a(2)  = " << a.Eval(2) << endl;      // 測試 Eval：把 x = 2 代入 a

    Polynomial z;                                  // 測試預設建構子：p(x) = 0
    cout << "zero  = " << z << endl;               // 測試零多項式的輸出
    return 0;
}
```

## 效能分析

設 `*this` 有 $m$ 個非零項、`poly` 有 $n$ 個非零項，$t$ 表示單一多項式的項數。

| 函式 | 時間複雜度 | 空間複雜度 |
|------|-----------|-----------|
| `Polynomial()` | $O(1)$ | $O(1)$ |
| 複製建構子、`operator=`、解構子 | $O(t)$、$O(t)$、$O(1)$ | $O(t)$ |
| `newTerm` | 均攤 $O(1)$（擴容的那一次為 $O(t)$） | $O(t)$（擴容時需暫存新陣列） |
| `Add` | $O(m+n)$ | $O(m+n)$（結果最多 $m+n$ 項） |
| `Mult` | $O(m^2n)$ | $O(mn)$ |
| `Eval` | $O(m)$ | $O(1)$ |
| `operator<<` | $O(t)$ | $O(1)$ |
| `operator>>`（讀入 $t$ 項） | $O(t^2)$ | $O(t)$ |

### 時間複雜度說明

1. **`Add`**：`aPos` 與 `bPos` 只會往後移動，每次迴圈至少前進一格，合計最多執行 $m+n$ 次，每次為 $O(1)$（`newTerm` 為均攤 $O(1)$），所以是 $O(m+n)$。
2. **`Mult`**：外層迴圈執行 $m$ 次。第 $i$ 次迴圈中，建立 $t$ 需要 $O(n)$；此時 $c$ 最多有 $(i-1)n$ 項，`c.Add(t)` 與接著的 `operator=` 複製都是 $O(i\cdot n)$。因此

   $$T(m,n) = \sum_{i=1}^{m} O(i \cdot n) = O\left(n \cdot \frac{m(m+1)}{2}\right) = O(m^2 n)$$

   最壞情況發生在乘積的指數幾乎沒有重複（結果有 $mn$ 項）。若兩個多項式都是各次項皆存在的稠密多項式，結果最多只有 $m+n-1$ 項，時間會降為 $O(m(m+n))$。
3. **`Eval`**：每個非零項做一次乘法與一次 `pow`，若將 `pow` 視為 $O(1)$，則為 $O(m)$；若 `pow` 以快速冪實作，則為 $O(m \log e_{max})$。
4. **`operator>>`**：第 $i$ 項併入時 `Add` 的成本為 $O(i)$，總和為 $O(t^2)$。輸入規模通常很小，換得「輸入不必排序」的方便性。

### 空間複雜度說明

- 一個 `Polynomial` 的空間為 $O(\text{capacity})$。因為擴容時容量加倍，所以 $\text{capacity} < 2 \cdot \text{terms}$（`terms` $\geq 1$ 時），即 $O(t)$。
- `Mult` 執行時同時存在結果 $c$（最多 $mn$ 項）、暫存的 $t$（$n$ 項）與 `Add` 回傳的暫存多項式（最多 $mn$ 項），總計為 $O(mn)$。

## 測試與驗證

### 測試案例

輸入格式為「項數 係數 指數 係數 指數 …」，例如 `3 3 2 2 1 1 0` 代表 $3x^2 + 2x + 1$。`Eval` 一律以 $x = 2$ 代入。

| 測試案例 | 測試目的 | 輸入 | 預期輸出 | 實際輸出 |
|----------|----------|------|----------|----------|
| 測試一 | 一般情況：$a = 3x^2+2x+1$、$b = x-1$（加法中有項相消） | `a: 3 3 2 2 1 1 0`<br>`b: 2 1 1 -1 0` | `a+b = 3x^2 + 3x`<br>`a*b = 3x^3 - x^2 - x - 1`<br>`a(2) = 17` | `a+b = 3x^2 + 3x`<br>`a*b = 3x^3 - x^2 - x - 1`<br>`a(2) = 17` |
| 測試二 | 零多項式：$a = 0$（項數為 0）、$b = 2x+1$ | `a: 0`<br>`b: 2 2 1 1 0` | `a = 0`<br>`a+b = 2x + 1`<br>`a*b = 0`<br>`a(2) = 0` | `a = 0`<br>`a+b = 2x + 1`<br>`a*b = 0`<br>`a(2) = 0` |
| 測試三 | 完全抵銷：$a = x^2+1$、$b = -x^2-1$ | `a: 2 1 2 1 0`<br>`b: 2 -1 2 -1 0` | `a+b = 0`<br>`a*b = -x^4 - 2x^2 - 1`<br>`a(2) = 5` | `a+b = 0`<br>`a*b = -x^4 - 2x^2 - 1`<br>`a(2) = 5` |
| 測試四 | 輸入未排序、有重複指數：$a = 1 + 2x^3 + 5x^3 - 1 = 7x^3$ | `a: 4 1 0 2 3 5 3 -1 0`<br>`b: 2 -7 3 1 1` | `a = 7x^3`<br>`b = -7x^3 + x`<br>`a+b = x`<br>`a*b = -49x^6 + 7x^4`<br>`a(2) = 56` | `a = 7x^3`<br>`b = -7x^3 + x`<br>`a+b = x`<br>`a*b = -49x^6 + 7x^4`<br>`a(2) = 56` |
| 測試五 | 常數項與高次項：$a = 5$、$b = x^{100}$ | `a: 1 5 0`<br>`b: 1 1 100` | `b = x^100`<br>`a+b = x^100 + 5`<br>`a*b = 5x^100`<br>`a(2) = 5` | `b = x^100`<br>`a+b = x^100 + 5`<br>`a*b = 5x^100`<br>`a(2) = 5` |
| 測試六 | 負係數與係數 $\pm 1$：$a = -x^2+x-1$、$b = x+1$ | `a: 3 -1 2 1 1 -1 0`<br>`b: 2 1 1 1 0` | `a = -x^2 + x - 1`<br>`a+b = -x^2 + 2x`<br>`a*b = -x^3 - 1`<br>`a(2) = -3` | `a = -x^2 + x - 1`<br>`a+b = -x^2 + 2x`<br>`a*b = -x^3 - 1`<br>`a(2) = -3` |
| 測試七 | 浮點數係數：$a = 0.5x^2+1.5$、$b = 2x^2$ | `a: 2 0.5 2 1.5 0`<br>`b: 1 2 2` | `a+b = 2.5x^2 + 1.5`<br>`a*b = x^4 + 3x^2`<br>`a(2) = 3.5` | `a+b = 2.5x^2 + 1.5`<br>`a*b = x^4 + 3x^2`<br>`a(2) = 3.5` |

另外，也以 `-fsanitize=address,undefined` 編譯並執行上述七組測試，沒有偵測到記憶體錯誤，確認複製建構子與 `operator=` 的深層複製沒有重複釋放或記憶體洩漏的問題。

### 編譯與執行指令

`a:`、`b:` 後面的數字為使用者輸入，其餘為程式輸出。

**測試一**

```shell
$ g++ -std=c++17 -o main main.cpp
$ ./main
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
$ ./main
輸入格式: 項數 係數 指數 係數 指數 ...  (例: 3 3 2 2 1 1 0 代表 3x^2+2x+1)
a: 4 1 0 2 3 5 3 -1 0
b: 2 -7 3 1 1
a     = 7x^3
b     = -7x^3 + x
a+b   = x
a*b   = -49x^6 + 7x^4
a(2)  = 56
zero  = 0
```

最後一行 `zero = 0` 是主程式中另外建立一個預設建構的 `Polynomial z` 並輸出，用來驗證預設建構子確實建立 $p(x) = 0$。

### 結論

1. `Add`、`Mult`、`Eval` 的結果皆與手算相符，包含一般情況、零多項式、完全抵銷、負係數與浮點數係數。
2. 輸入未排序、出現重複指數時，`>>` 仍能正確合併並維持「指數遞減、無重複」的性質，`<<` 也能正確處理零多項式與係數 $\pm 1$ 的格式。
3. 測試涵蓋了邊界情況（零多項式、高次項 $x^{100}$、項相消成 0），並以 AddressSanitizer 驗證沒有記憶體錯誤。

## 申論及開發報告

### 為何使用「非零項陣列」（稀疏表示）

多項式也可以用「陣列的第 $i$ 格存 $x^i$ 的係數」表示，這種稠密表示法的優點是取係數很快。但像 $x^{1000} + 1$ 這種多項式只有兩個非零項，稠密表示需要 1001 格，`Add` 也要掃過全部 1001 格。稀疏表示只存非零項，空間為 $O(\text{非零項數})$，運算時間也只和非零項數有關。代價是無法直接用指數當索引，要找特定指數的係數需要搜尋。題目的 Figure 2 本來就是這種表示法，也正好適合處理次數很高但項數很少的多項式。

### 為何維持「指數由大到小」排序

`Add` 之所以能用合併做到 $O(m+n)$，前提就是兩邊都已排序。如果不排序，每一項都得到另一個多項式中搜尋同指數的項，時間會變成 $O(mn)$。排序也讓 `<<` 可以直接由高次項輸出到低次項。

要維持排序，程式有兩個設計：`newTerm` 只在尾端新增，呼叫者（`Add`、`Mult`）本身就是依遞減順序產生；而使用者輸入的項，則是透過 `Add` 併入，不直接插入陣列，因此即使輸入亂序也不會破壞性質。

### 為何 `newTerm` 擴容時將容量加倍

```cpp
if (terms == capacity) {
    capacity *= 2;
    Term *temp = new Term[capacity];
    copy(termArray, termArray + terms, temp);
    delete [] termArray;
    termArray = temp;
}
```

如果每次只增加固定大小 $c$，新增 $t$ 項的總搬移次數約為 $t^2/(2c)$，均攤下來每次新增是 $O(t)$。若採用加倍，總搬移次數為 $1 + 2 + 4 + \cdots + t < 2t$，均攤每次新增只要 $O(1)$。代價是最多浪費約一半的空間。

### 為何需要複製建構子與 `operator=`

`Polynomial` 內含指標 `termArray`，如果不自己寫，編譯器產生的版本只會複製指標，造成兩個物件指向同一塊記憶體。`Add` 與 `Mult` 以傳值回傳，區域變數離開作用域時解構子會 `delete[]` 該陣列，回傳的物件就變成懸空指標，最後還會重複釋放。因此依照 Rule of Three，只要需要自訂解構子，就一併實作複製建構子與 `operator=`，並採用深層複製。`operator=` 另外加入 `this != &other` 判斷，並採用「先配置新陣列、再釋放舊陣列」的順序，處理自我指定的情況。

### 為何 `<<`、`>>` 使用 friend，且 `Term` 也要宣告 friend

`cout << p` 的左運算元是 `ostream`，因此運算子不能是 `Polynomial` 的成員函式，只能是一般函式；而它需要讀取 `Polynomial` 的私有成員，所以宣告為 `friend`。另外，`friend` 的關係不具傳遞性：`Polynomial` 是 `Term` 的 friend，並不代表 `Polynomial` 的 friend 函式也能存取 `Term` 的私有成員，所以 `operator<<` 也要在 `Term` 內另外宣告。回傳 `ostream&`、`istream&` 則是為了支援 `cout << a << b` 的串接寫法。

### 與 ADT 宣告不同之處

Figure 1 中 `Add(Polynomial poly)` 是傳值，每次呼叫都會多做一次完整複製。本程式改為 `const Polynomial&`，語意相同（呼叫端的使用方式不變），但省去複製；同時將 `Add`、`Mult`、`Eval` 宣告為 `const` 成員函式，表示不會修改 `*this`，使 `const` 物件也能呼叫。

### 限制與可改進之處

1. **`Mult` 的效率**：目前為 $O(m^2n)$。可以改成先把 $mn$ 個乘積全部算出，排序後再合併同指數項，時間為 $O(mn\log(mn))$，在項數很多時更快。
2. **`Eval` 的效率**：目前每項都呼叫 `pow`。可以改用稀疏版的 Horner's rule：由高次項往低次項處理，每次乘上 $f^{e_i - e_{i+1}}$，減少乘法次數。
3. **浮點數誤差**：`newTerm` 以 `theCoef == 0` 判斷係數是否為 0。`float` 運算可能因誤差，讓應該抵銷的項剩下極小的非零值而殘留。可以改用容許誤差（例如 `fabs(x) < 1e-6`），但門檻值的選擇會影響很小的合法係數，需要取捨。
4. **輸入檢查**：程式假設輸入的指數為非負整數、項數為合法值（符合題目的定義），沒有額外檢查。
