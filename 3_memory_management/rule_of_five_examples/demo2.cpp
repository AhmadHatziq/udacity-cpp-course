#include <iostream>
#include <utility> // For std::move


// A class that manages a dynamically allocated integer.
class MyDynamicClass {
public:
    // Constructor: Allocates memory and initializes the value.
    MyDynamicClass(int value) {
        std::cout << "Constructor called. Allocating new memory." << std::endl;
        m_data = new int(value);
    }


    // Destructor: Frees the dynamically allocated memory.
    ~MyDynamicClass() {
        if (m_data != nullptr) {
            std::cout << "Destructor called. Freeing memory at address " << m_data << std::endl;
            delete m_data;
        } else {
            std::cout << "Destructor called. Object was moved, no memory to free." << std::endl;
        }
    }


    // Copy Constructor: Performs a deep copy.
    MyDynamicClass(const MyDynamicClass& other) {
        std::cout << "Copy constructor called. Performing a deep copy." << std::endl;
        m_data = new int(*other.m_data);
    }


    // Copy Assignment Operator: Performs a deep copy.
    MyDynamicClass& operator=(const MyDynamicClass& other) {
        std::cout << "Copy assignment operator called." << std::endl;
        if (this != &other) {
            delete m_data;
            m_data = new int(*other.m_data);
        }
        return *this;
    }


    // Move Constructor: Steals the resources from another object.
    // The 'other' parameter is an rvalue reference (&&).
    MyDynamicClass(MyDynamicClass&& other) noexcept {
        std::cout << "Move constructor called. Stealing resources." << std::endl;
        // Steal the pointer from the source object.
        m_data = other.m_data;
        // Null out the source's pointer so its destructor doesn't free the memory.
        other.m_data = nullptr;
    }


    // Move Assignment Operator: Steals the resources from another object.
    MyDynamicClass& operator=(MyDynamicClass&& other) noexcept {
        std::cout << "Move assignment operator called." << std::endl;
        // Check for self-assignment.
        if (this != &other) {
            // First, free our own memory.
            delete m_data;
            // Then, steal the pointer from the source.
            m_data = other.m_data;
            // Null out the source's pointer.
            other.m_data = nullptr;
        }
        return *this;
    }


    // Member function to print the value and memory address.
    void printValue() const {
        if (m_data != nullptr) {
            std::cout << "Value: " << *m_data << ", Memory address: " << m_data << std::endl;
        } else {
            std::cout << "Value: (nullptr), Memory address: (nullptr) - Object has been moved." << std::endl;
        }
    }


private:
    int* m_data; // A pointer to a dynamically allocated integer.
};


// A function that returns a temporary object (an rvalue).
MyDynamicClass createTempObject() {
    MyDynamicClass temp(99);
    return temp;
}


int main() {
    // 1. Automatic move from a temporary object (rvalue).
    std::cout << "--- 1. Automatic Move from a Temporary ---" << std::endl;
    // The object returned by createTempObject() is a temporary.
    // The compiler automatically calls the move constructor to avoid a deep copy.
    MyDynamicClass movedObj = createTempObject();
    movedObj.printValue();
    std::cout << "------------------------------------------" << std::endl;


    // 2. Explicit move using std::move.
    std::cout << "\n--- 2. Explicit Move using std::move ---" << std::endl;
    MyDynamicClass original(10);
    original.printValue();
   
    // We explicitly tell the compiler to treat 'original' as an rvalue,
    // which forces the move constructor to be called.
    MyDynamicClass movedFromOriginal = std::move(original);
    movedFromOriginal.printValue();


    // The 'original' object has now been "moved from."
    // It's in a valid but unspecified state, so we must not use its data.
    original.printValue();
    std::cout << "------------------------------------------" << std::endl;


    // 3. Move Assignment.
    std::cout << "\n--- 3. Move Assignment Demo ---" << std::endl;
    MyDynamicClass existingObj(20);
    existingObj.printValue();


    // Assigning a temporary object to 'existingObj' will call the move assignment operator.
    existingObj = createTempObject();
    existingObj.printValue();
    std::cout << "------------------------------------------" << std::endl;


    return 0;
}
