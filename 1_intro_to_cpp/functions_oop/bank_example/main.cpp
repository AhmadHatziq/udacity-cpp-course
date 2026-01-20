#include <iostream>
#include "bank_account.h"

int main() {
    BankAccount acc1;  // Default constructor
    acc1.showInfo();

    std::cout << "\n";

    BankAccount acc2("Luffy", 1250.50);  // Parameterized constructor
    acc2.showInfo();

    std::cout << "\nDepositing $250 into Luffy's account...\n";
    acc2.deposit(250);
    acc2.showInfo();
    return 0;
}