/*
Write a program that:

    Asks the user to enter their age (int)

    Asks for the number of tickets they want (int)

    Then asks for their full name (std::getline)

    Then asks for their favorite movie title (std::getline)

    Prints a reservation summary
*/

#include <iostream>
#include <string>

int main() {
    int age;
    int tickets;
    std::string name;
    std::string movie;

    // Use cin to get the relevant information from the user
    std::cout << "Enter your age: ";
    std::cin >> age;

    std::cout << "How many tickets? ";
    std::cin >> tickets;

    std::cin.ignore(); // flush the newline character after cin
    std::cout << "Enter your full name: ";
    std::getline(std::cin, name); // use getline to get the full answer wiht the space

    std::cout << "Enter your favorite movie: ";
    std::getline(std::cin, movie);

    // Print the ticket summary
    std::cout << "\n Ticket Summary:\n";
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Tickets: " << tickets << std::endl;
    std::cout << "Favorite Movie: " << movie << std::endl;

    return 0;
}
