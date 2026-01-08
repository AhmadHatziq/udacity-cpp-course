#include <iostream>
#include <vector>

int main() {
    std::vector<int> pages;
    int input;
    int total = 0;

    std::cout << "Enter pages read each day (7 days):\n";

    // Take user input
    for (int i = 0; i < 7; ++i) {
        std::cout << "Day " << (i + 1) << ": ";
        std::cin >> input;
        pages.push_back(input);
    }

    // Print the pages read each day
    std::cout << "\nPages read each day:\n";
    for (int i = 0; i < pages.size(); ++i) {
        std::cout << "Day " << (i + 1) << ": " << pages[i] << " pages\n";
        total += pages[i]; // in the array solution we used total = total + 1, in c++ you can use += to perform this calculation as well 
    }

    // Print total pages
    std::cout << "\nTotal pages: " << total << std::endl;

    return 0;
}
