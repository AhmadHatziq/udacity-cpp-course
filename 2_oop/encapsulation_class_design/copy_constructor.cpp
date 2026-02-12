#include <iostream>
#include <cstring>
using namespace std;

class Employee {
private:
    int id;
    char* name;

public:
    // Constructor
    Employee(int id_, const char* initName) : id(id_) {
        name = new char[strlen(initName) + 1];
        strcpy(name, initName);
    }

    // Destructor
    ~Employee() {
        delete[] name;
    }

    // Copy Constructor (deep copy)
    Employee(const Employee& other) : id(other.id) {
        name = new char[strlen(other.name) + 1];
        strcpy(name, other.name);
        cout << "Copy constructor called\n";
    }

    // Copy Assignment (deep copy + self-assignment guard)
    Employee& operator=(const Employee& other) {
        if (this == &other) return *this; // self-assignment check
        delete[] name;                    // free old memory
        id = other.id;
        name = new char[strlen(other.name) + 1];
        strcpy(name, other.name);
        cout << "Copy assignment called\n";
        return *this;
    }

    void print() const {
        cout << "Employee " << id << ": " << name << "\n";
    }
};

int main() {
    Employee e1(1, "Alice");
    e1.print();

    cout << "\n-- Copy constructing e2 from e1 --\n";
    Employee e2 = e1;  // Calls copy constructor
    e2.print();

    cout << "\n-- Copy assigning e3 from e1 --\n";
    Employee e3(3, "Temp");
    e3 = e1;           // Calls copy assignment
    e3.print();

    cout << "\n-- Modifying e2's name shouldn't affect e1 --\n";
    // To test, we assign again with different name
    Employee e4(4, "Bob");
    e2 = e4;
    e1.print();
    e2.print();
}
