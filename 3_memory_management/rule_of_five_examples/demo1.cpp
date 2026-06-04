#include <iostream>


// A class that manages a dynamically allocated integer.
class MyDynamicClass {
public:
    // Constructor: Allocates memory and initializes the value.
    MyDynamicClass(int value) {
        std::cout << "Constructor called. Allocating new memory." << std::endl;
        m_data = new int(value);
    }


    // 1. Destructor: Frees the dynamically allocated memory.
    // This is called automatically when an object of this class goes out of scope.
    ~MyDynamicClass() {
        std::cout << "Destructor called. Freeing memory at address " << m_data << std::endl;
        delete m_data;
    }


    // 2. Copy Constructor: Creates a new object as a deep copy of another.
    // It is called when an object is initialized with another object of the same type.
    // E.g., MyDynamicClass newObj = oldObj;
    MyDynamicClass(const MyDynamicClass& other) {
        std::cout << "Copy constructor called. Performing a deep copy." << std::endl;
        // Allocate new memory for this object's data.
        m_data = new int;
        // Copy the value, not the pointer.
        *m_data = *other.m_data;
    }


    // 3. Copy Assignment Operator: Assigns one object's value to another.
    // It is called when an object is assigned to another.
    // E.g., existingObj = otherObj;
    MyDynamicClass& operator=(const MyDynamicClass& other) {
        std::cout << "Copy assignment operator called." << std::endl;
        // Check for self-assignment to prevent errors (e.g., obj = obj;).
        if (this != &other) {
            std::cout << "Freeing old memory and performing a deep copy." << std::endl;
            // 1. Free the memory currently owned by this object.
            delete m_data;
            // 2. Allocate new memory.
            m_data = new int;
            // 3. Copy the value from the source object.
            *m_data = *other.m_data;
        }
        return *this;
    }


    // Member function to print the value and memory address.
    void printValue() const {
        std::cout << "Value: " << *m_data << ", Memory address: " << m_data << std::endl;
    }


    int* m_data; // A pointer to a dynamically allocated integer.
};


int main() {
    // Demonstrate the constructor and destructor
    std::cout << "--- 1. Constructor and Destructor Demo ---" << std::endl;
    MyDynamicClass original(10);
    original.printValue();


    std::cout << "\n--- 2. Copy Constructor Demo ---" << std::endl;
    // This calls the copy constructor.
    MyDynamicClass copiedObj = original;
    copiedObj.printValue();


    // Change the original object's value. The copy remains unchanged.
    // This proves it's a deep copy, not a shallow copy.
    *original.m_data = 20;
    std::cout << "After changing original value:" << std::endl;
    original.printValue();
    copiedObj.printValue();


    std::cout << "\n--- 3. Copy Assignment Operator Demo ---" << std::endl;
    MyDynamicClass anotherObj(30);
    anotherObj.printValue();


    // This calls the copy assignment operator.
    anotherObj = original;
    anotherObj.printValue();


    std::cout << "\nEnd of main function. Objects will now be destroyed." << std::endl;
    // The destructors for original, copiedObj, and anotherObj will be called here.


    return 0;
}
