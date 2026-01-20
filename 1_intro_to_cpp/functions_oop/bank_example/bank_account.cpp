#include <iostream>
#include "bank_account.h"

// Default constructor
BankAccount::BankAccount() {
    std::cout << "[Constructor] Creating default bank account...\n";
    accountHolder = "Unnamed";
    balance = 0.0;
}

// Parameterized constructor
BankAccount::BankAccount(std::string holder, double bal) {
    std::cout << "[Constructor] Creating account for " << holder << "\n";
    accountHolder = holder;
    balance = bal;
}

// Destructor
BankAccount::~BankAccount() {
    std::cout << "[Destructor] Closing account for " << accountHolder << "\n";
}

// Method to show account info
void BankAccount::showInfo() const {
    std::cout << "Account Holder: " << accountHolder << "\n";
    std::cout << "Balance: $" << balance << "\n";
}

// Deposit method
void BankAccount::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        std::cout << "[Deposit] $" << amount << " added to " << accountHolder << "'s account.\n";
    } else {
        std::cout << "[Deposit] Invalid deposit amount.\n";
    }
}