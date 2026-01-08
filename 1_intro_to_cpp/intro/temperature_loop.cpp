#include <iostream>

int main() {
    int choice;
    double temp;
    char convertAgain = 'y'; 

    while(convertAgain == 'y' || convertAgain == 'Y') {
        std::cout << "Convert:\n1. Celsius to Fahrenheit\n2. Fahrenheit to Celsius\nEnter your choice (1 or 2): ";
        std::cin >> choice;

        // if Celsius to Farhenheit
        if (choice == 1) {
            // convert to Fahrenheit and print to console
            // F = C*(9/5) + 32
            std::cout << "Enter temperature in Celsius: ";
            std::cin >> temp;
            double fahrenheit = (temp * (9.0 / 5)) + 32;
            std::cout << "In Fahrenheit: " << fahrenheit << std::endl;
        } else if (choice == 2) {
            // convert to Celsius and print to console
            // C = (F-32)*(5/9)
            std::cout << "Enter temperature in Fahrenheit: ";
            std::cin >> temp;
            double celsius = (temp - 32) * (5.0 / 9);
            std::cout << "In Celsius: " << celsius << std::endl;
        }     
        else {
            std::cout << "Invalid choice!" << std::endl;
        }

        std::cout << "\nDo you want to convert another temperature? (y/n): ";
        std::cin >> convertAgain;
        std::cout << std::endl;
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
    /*
    Bonus Challenge (Optional)
    Add a loop so that after converting one temperature, the program asks the user if they want to convert another.
    (Use a while loop with a char convertAgain = 'y' check.)
    */
}