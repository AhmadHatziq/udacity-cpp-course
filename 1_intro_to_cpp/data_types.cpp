#include <iostream>
#include <string>

int main() {
    // Initialize all the relevant variables
    int siblings = 3;
    double temperature = 36.65;
    char middleInitial = 'J';
    bool passedExam = true;
    std::string fullName = "Alex Johnson";
    double pi = 3.141592653589793;

    // Print the sentences
    std::cout << fullName << " has " << siblings << " siblings." << std::endl;
    std::cout << "The average temperature is " << temperature << " degrees Celsius." << std::endl;
    std::cout << "Their middle initial is " << middleInitial << "." << std::endl;
    std::cout << "They passed the exam: " << std::boolalpha << passedExam << "." << std::endl;
    std::cout << "Pi is approximately " << pi << "." << std::endl;

    return 0;
}
