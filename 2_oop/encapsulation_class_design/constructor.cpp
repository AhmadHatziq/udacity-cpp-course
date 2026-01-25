#include <iostream>
#include <string>
using namespace std;

class Employee {
private:
    string company;
    string surname;
    string givenName;

public:
    // Default constructor with initializer list
    Employee() : company("TechCorp"), surname(""), givenName("") {
        cout << "Default constructor called\n";
    }

    // Parameterized constructor (surname + givenName)
    Employee(const string& initSurname, const string& initGivenName)
        : company("TechCorp"), surname(initSurname), givenName(initGivenName) {
        cout << "Parameterized constructor (2 args) called\n";
    }

    // Overloaded constructor (surname + givenName + company)
    Employee(const string& initSurname, const string& initGivenName, const string& initCompany)
        : company(initCompany), surname(initSurname), givenName(initGivenName) {
        cout << "Overloaded constructor (3 args) called\n";
    }

    // Constructor chaining (surname only)
    Employee(const string& initSurname)
        : Employee(initSurname, "Unknown") {
        cout << "Constructor chaining called\n";
    }

    // Helper to print employee info
    void print() const {
        cout << "Company: " << company
             << ", Employee: " << givenName << " " << surname << "\n";
    }
};

int main() {
    cout << "\n== Default constructor ==\n";
    Employee e1;
    e1.print();

    cout << "\n== Parameterized constructor ==\n";
    Employee e2("Doe", "John");
    e2.print();

    cout << "\n== Overloaded constructor ==\n";
    Employee e3("Roe", "Jane", "DataWorks");
    e3.print();

    cout << "\n== Constructor chaining ==\n";
    Employee e4("Smith");
    e4.print();
}
