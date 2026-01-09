#include <iostream>
#include <string>

// Overloaded versions of the `printUserInfo` function

void printUserInfo(int age) {
    std::cout << "Age: " << age << " years old\n";
}

void printUserInfo(std::string name) {
    std::cout << "Name: " << name << "\n";
}

void printUserInfo(std::string name, int age) {
    std::cout << "Name: " << name << ", Age: " << age << "\n";
}

void printUserInfo(double height) {
    std::cout << "Height: " << height << " meters\n";
}

int main() {
    std::cout << "Function Overloading Demo:\n\n";

    printUserInfo(25);                     // Calls version with int
    printUserInfo("Alice");               // Calls version with string
    printUserInfo("Bob", 30);            // Calls version with string and int
    printUserInfo(1.75);                 // Calls version with double
    printUserInfo(30, "Bob");

    return 0;
}