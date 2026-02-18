#include <iostream>
#include <string>
#include <utility>
#include <algorithm>  // for std::max

template <typename T>
T max3(const T& a, const T& b, const T& c) {return std::max(a, std::max(b, c));}

template <typename T>
struct Box {
    static inline int count = 0;  // one counter per T type
    T v;
    explicit Box(T x) : v(std::move(x)) {++count;}
};

int main() {
    std::cout << max3(1, 5, 3) << "\n";               // int -> 5
    std::cout << max3(2.1, 4.8, 3.0) << "\n";         // double -> 4.8
    std::cout << max3<double>(1, 2.0, 3) << "\n";     // explicit <double> -> 3

    Box<int> bi{42};
    Box<std::string> bs{"hi"};
    std::cout << Box<int>::count << " " << Box<std::string>::count << "\n"; // 1 1
    return 0;
}