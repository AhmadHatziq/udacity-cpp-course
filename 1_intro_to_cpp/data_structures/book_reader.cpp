#include <iostream>

int main() {
    int pages[7] = {10, 15, 12, 14, 15, 40, 60};
    int total = 0;

    // Print pages for each day of the week
    for (int i = 0; i < 7; i++) {
        std::cout << "Day " << (i + 1) << ": " << pages[i] << std::endl;
        // Keep a track of total pages read
        total = total + pages[i];
    }

    // Print total pages
    std::cout << "\nTotal pages: " << total << std::endl;

    return 0;
}
