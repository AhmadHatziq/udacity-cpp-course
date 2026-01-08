#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::ifstream file("logs.txt");
    if (!file) {
        std::cerr << "Could not open log file." << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string timestamp, level, message;

        std::getline(ss, timestamp, ','); // As we know the line is comma seperated
        std::getline(ss, level, ',');
        std::getline(ss, message);  // read the rest of the line

        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << "Level:     " << level << std::endl;
        std::cout << "Message:   " << message << std::endl;

        if (level == "ERROR") {
            std::cout << "!!! This is an error.\n";
        }

        std::cout << "----------------------\n";
    }

    return 0;
}
