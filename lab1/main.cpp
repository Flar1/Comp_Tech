#include <iostream>
#define ofds 1
int main() {
    int cols_s, rows_s, cols_e, rows_e;
    std::cin >> cols_s >> rows_s >> cols_e >> rows_e;
    int a;
    bool def;
    int d = 42;
    int o = 052;
    int x = 0x2a;
    int X = 0X2A;
    int b = 0b101010; 
    unsigned long long l1 = 18446744073709550592ull;       
    unsigned long long l2 = 18'446'744'073'709'550'592llu; 
    unsigned long long l3 = 1844'6744'0737'0955'0592uLL;   
    unsigned long long l4 = 184467'440737'0'95505'92LLU; 
    / Floating-point literals
float f = 3.4028234e38f;
double d0 = 58.;
double d1 = 4e2;
double d2 = 123.456e-67;
double d3 = .1;
double d4 = .1E4f;
double d = 2.4028234e38;
double d = 0x1.4p3; // hex fraction 1.4 (decimal 1.25) scaled by 23, that is 10.0
long double -1.4028234e38l;
std::bfloat16_t bfc = 0.1f64BF16; // C++23А
char *s = "s-char-seq";
const wchar_t* s5 = L"ABCDEF"
    return 0;
}
