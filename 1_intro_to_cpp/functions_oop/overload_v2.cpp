#include <iostream>
#include <string>

// Overload 1: Describe by name
void describe(const std::string& name) {
    std::cout << "Character: " << name << std::endl;
}

// Overload 2: Describe by level
void describe(int level) {
    std::cout << "Level: " << level << std::endl;
}

// Overload 3: Describe by location
void describe(double x, double y) {
    std::cout << "Location: (" << x << ", " << y << ")" << std::endl;
}

// Overload 4: Describe by status
void describe(bool isAlive) {
    std::cout << "Status: " << (isAlive ? "Alive" : "Dead") << std::endl;
}

int main() {
    std::string name = "Asha";
    int level = 7;
    double x = 13.2, y = 42.9;
    bool alive = true;

    describe(name);      // Character: Asha
    describe(level);     // Level: 7
    describe(x, y);      // Location: (13.2, 42.9)
    describe(alive);     // Status: Alive

    return 0;
}
