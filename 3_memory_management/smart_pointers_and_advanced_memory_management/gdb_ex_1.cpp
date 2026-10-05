#include <iostream>

void print_sum_info(int n) {
      int sum = 0;
      for (int i = 1; i <= n; ++i) {
          sum += i;
      }
      std::cout << "The sum of the first " << n << " integers is: " << sum << std::endl;
}

int main() {
      int number = 5;
      print_sum_info(number);
      return 0;
}