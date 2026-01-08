#include <iostream>

int main() {
    int goal = 750;
    int month = 0;
    // Start savings at 0
    int savings = 0;

    // Until we reach out goal
    while (savings < goal) {
        month++;
        // Update the savings to add $100 every month
        savings += 100;
        // Print the month and the amount in savings
        std::cout << "Month " << month << ": $" << savings << std::endl;
    }

    std::cout << "You reached your goal in " << month << " months!" << std::endl;

    return 0;
}
