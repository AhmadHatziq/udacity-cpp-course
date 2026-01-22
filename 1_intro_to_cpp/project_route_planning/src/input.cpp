#include <iostream>

int main() {
    float start_x = 0.0, start_y = 0.0, end_x = 0.0, end_y = 0.0;

    std::cout << "Enter start_x start_y end_x end_y: ";

    if (!(std::cin >> start_x >> start_y >> end_x >> end_y)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    std::cout << "You entered:\n";
    std::cout << "start_x = " << start_x << "\n";
    std::cout << "start_y = " << start_y << "\n";
    std::cout << "end_x   = " << end_x   << "\n";
    std::cout << "end_y   = " << end_y   << "\n";

    return 0;
}