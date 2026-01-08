#include <iostream>
#include <string>

int main()
{
    int option;
    std::string comment;

    // integer from 1-5
    std::cout << "Enter a menu option: ";
    std::cin >> option;

    std::cin.ignore(1000, '\n'); //Ignore leftover characters 
    std::cout << "Enter your allergies: ";
    std::getline(std::cin, comment);

    std::cout << "You chose option " << option << " and your allergies are: " << comment << std::endl;

    return 0;
}