#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <string>

class BankAccount {
private:
    std::string accountHolder;
    double balance;

public:
    BankAccount();                               // Default constructor
    BankAccount(std::string holder, double bal); // Parameterized constructor
    ~BankAccount();                              // Destructor

    void deposit(double amount);                 // New method
    void showInfo() const;
};

#endif