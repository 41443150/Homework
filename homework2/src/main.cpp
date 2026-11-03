#include <iostream>
#include <cmath>      // pow
#include <algorithm>  // copy
using namespace std;

class Polynomial;

// Term：多項式中的一項 (係數 coef, 指數 exp)
class Term {
    friend class Polynomial;                                 // 讓 Polynomial 能存取 private 成員
    friend ostream& operator<<(ostream&, const Polynomial&); // friend 不具傳遞性，輸出函式也要單獨宣告
private:
    float coef;   // 係數
    int   exp;    // 指數 (非負整數)
};

//Polynomial
class Polynomial {
    // 輸入/輸出運算子需要存取 private 成員，所以宣告為 friend
    friend ostream& operator<<(ostream&, const Polynomial&);
    friend istream& operator>>(istream&, Polynomial&);
private:
    Term *termArray;  // 動態陣列，存放非零項
    int capacity;     // termArray 的大小
    int terms;        // 目前實際有幾個非零項

    // 在陣列尾端新增一項；呼叫者需保證指數比現有各項都小 (維持遞減順序)
    void newTerm(const float theCoef, const int theExp);
public:
    Polynomial();                                     // 建立 p(x) = 0
    Polynomial(const Polynomial& other);              // 複製建構子
    Polynomial& operator=(const Polynomial& other);   // 指定運算子
    ~Polynomial();                                    // 釋放動態記憶體

    Polynomial Add(const Polynomial& poly) const;     // 回傳 *this + poly
    Polynomial Mult(const Polynomial& poly) const;    // 回傳 *this * poly
    float Eval(float f) const;                        // 回傳 p(f)
};

// 建構子：建立零多項式 (沒有任何項)，初始容量為 1
Polynomial::Polynomial() : capacity(1), terms(0) {
    termArray = new Term[capacity];
}

// 複製建構子
Polynomial::Polynomial(const Polynomial& other)
    : capacity(other.capacity), terms(other.terms) {
    termArray = new Term[capacity];
    for (int i = 0; i < terms; i++)
        termArray[i] = other.termArray[i];
}

// 指定運算子：先配置並複製新資料，成功後才釋放舊資料
Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this != &other) {
        Term *temp = new Term[other.capacity];
        for (int i = 0; i < other.terms; i++)
            temp[i] = other.termArray[i];
        delete [] termArray; // 釋放舊陣列
        termArray = temp;
        capacity  = other.capacity;
        terms     = other.terms;
    }
    return *this;
}

// 解構子
Polynomial::~Polynomial() {
    delete [] termArray;
}

// newTerm：新增一項到陣列尾端。
// 係數為 0 的項不存 (維持「只存非零項」)
// 陣列已滿時，容量加倍再搬移資料 (平均每次新增為 O(1))
void Polynomial::newTerm(const float theCoef, const int theExp) {
    if (theCoef == 0) return;
    if (terms == capacity) {
        capacity *= 2;
        Term *temp = new Term[capacity];
        copy(termArray, termArray + terms, temp); // 搬移舊資料
        delete [] termArray;
        termArray = temp;
    }
    termArray[terms].coef = theCoef;
    termArray[terms++].exp = theExp;
}

// Add：兩個遞減排序的陣列做合併
// aPos / bPos 分別指向 *this 與 b 目前處理的項
// 時間複雜度 O(m + n)
Polynomial Polynomial::Add(const Polynomial& b) const {
    Polynomial c; // 存放結果
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

// Mult：*this 的每一項 × b 的所有項，產生一個暫存多項式 t
// 再用 Add 累加到結果 c
// t 的指數仍為遞減 (b 遞減，每項都加上同一個指數)，所以可以直接用 newTerm
// 最壞時間複雜度約 O(m^2 * n)，m、n 為兩多項式的項數
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

// Eval：計算 p(f) = Σ coef * f^exp，只需走訪非零項，時間 O(terms)
float Polynomial::Eval(float f) const {
    float result = 0;
    for (int i = 0; i < terms; i++)
        result += termArray[i].coef * pow(f, termArray[i].exp);
    return result;
}

// 輸出運算子 <<：輸出成類似 3x^2 + 2x - 1 的格式
// 特殊情況處理：零多項式輸出 0；係數為 ±1 時省略 1
// 指數為 1 時不印 ^1；指數為 0 時只印係數
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) return os << 0;               // 零多項式

    for (int i = 0; i < p.terms; i++) {
        float c = p.termArray[i].coef;
        int   e = p.termArray[i].exp;

        // 符號：第一項只有負號才印；之後的項用 " + " 或 " - " 連接
        if (i == 0) { if (c < 0) os << "-"; }
        else        os << (c < 0 ? " - " : " + ");

        float a = (c < 0) ? -c : c;                 // 係數取絕對值
        if (e == 0 || a != 1) os << a;              // 常數項一定要印；係數為 1 的 x 項省略
        if (e >= 1) os << "x";
        if (e >  1) os << "^" << e;
    }
    return os;
}

// 輸入運算子 >>：格式為「項數 n，接著 n 組 係數 指數」
// 例：3 3 2 2 1 1 0 代表 3x^2 + 2x + 1
// 每讀到一項就用 Add 併入 p，因此即使輸入沒排序
// 有重複指數也能維持「遞減、無重複」的性質
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

//主程式
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