#include <iostream>
#include <utility> // For std::move


// A class that manages a dynamically allocated integer.
// This is the same class from the previous demo to clearly show
// when copy/move constructors are called.
class MyDynamicClass {
public:
    MyDynamicClass(int value) {
        std::cout << "  Constructor called. Allocating new memory." << std::endl;
        m_data = new int(value);
    }


    ~MyDynamicClass() {
        if (m_data != nullptr) {
            std::cout << "  Destructor called. Freeing memory at address " << m_data << std::endl;
            delete m_data;
        } else {
            std::cout << "  Destructor called. Object was moved, no memory to free." << std::endl;
        }
    }


    // Copy Constructor: Performs a deep copy.
    MyDynamicClass(const MyDynamicClass& other) {
        std::cout << "  Copy constructor called. Performing a deep copy." << std::endl;
        m_data = new int(*other.m_data);
    }


    // Move Constructor: Steals resources.
    MyDynamicClass(MyDynamicClass&& other) noexcept {
        std::cout << "  Move constructor called. Stealing resources." << std::endl;
        m_data = other.m_data;
        other.m_data = nullptr;
    }


    void printValue() const {
        if (m_data != nullptr) {
            std::cout << "  Value: " << *m_data << ", Memory address: " << m_data << std::endl;
        } else {
            std::cout << "  Value: (nullptr), Memory address: (nullptr) - Object has been moved." << std::endl;
        }
    }


private:
    int* m_data;
};


// 1. Pass by Value
// This creates a *copy* of the object inside the function.
// The copy constructor is called.
void passByValue(MyDynamicClass obj) {
    std::cout << "  -- Inside passByValue() --" << std::endl;
    obj.printValue();
    std::cout << "  -- Exiting passByValue() --" << std::endl;
} // 'obj' destructor is called here, freeing the copy's memory.


// 2. Pass by Const Reference
// This passes a reference to the original object.
// No copy or move constructor is called, making it very efficient.
// 'const' prevents the function from modifying the original object.
void passByConstReference(const MyDynamicClass& obj) {
    std::cout << "  -- Inside passByConstReference() --" << std::endl;
    obj.printValue();
    std::cout << "  -- Exiting passByConstReference() --" << std::endl;
}


// 3. Pass by Rvalue Reference (for move)
// This accepts a temporary object (an rvalue) and allows for a move.
// The move constructor is called to efficiently "steal" the resources.
void passByRvalueReference(MyDynamicClass&& obj) {
    std::cout << "  -- Inside passByRvalueReference() --" << std::endl;
    obj.printValue();
    std::cout << "  -- Exiting passByRvalueReference() --" << std::endl;
}


int main() {
    std::cout << "--- Demo: Pass by Value (Triggers Copy) ---" << std::endl;
    MyDynamicClass original_copy(10);
    passByValue(original_copy);
    std::cout << "Original object after passByValue(): " << std::endl;
    original_copy.printValue();
    std::cout << "------------------------------------------" << std::endl;


    std::cout << "\n--- Demo: Pass by Const Reference (Avoids Copy) ---" << std::endl;
    MyDynamicClass original_ref(20);
    passByConstReference(original_ref);
    std::cout << "Original object after passByConstReference(): " << std::endl;
    original_ref.printValue();
    std::cout << "------------------------------------------------" << std::endl;


    std::cout << "\n--- Demo: Pass by Rvalue Reference (Triggers Move) ---" << std::endl;
    // We pass a temporary object, which is an rvalue.
    passByRvalueReference(MyDynamicClass(30));
    std::cout << "------------------------------------------" << std::endl;


    return 0;
}
