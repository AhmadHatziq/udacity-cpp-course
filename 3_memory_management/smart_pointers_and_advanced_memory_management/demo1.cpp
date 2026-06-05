#include <iostream>
#include <memory> // Header for std::unique_ptr
#include <utility> // For std::move


// This class demonstrates RAII by using a std::unique_ptr
// to manage a dynamically allocated integer array.
class DynamicMemoryHandler {
public:
    // Constructor: Acquires the resource (the memory block).
    // std::make_unique handles the allocation and returns a unique_ptr.
    DynamicMemoryHandler(size_t size) : m_size(size) {
        std::cout << "Constructor: Allocating a new integer array of size " << m_size << std::endl;
        m_data = std::make_unique<int[]>(m_size);


        // Initialize the memory to a known state.
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = i + 1;
        }
        std::cout << "Memory allocated. The unique_ptr now owns it." << std::endl;
    }


    // The destructor is not needed! The std::unique_ptr's destructor
    // automatically calls delete[] on the owned memory when this object
    // goes out of scope. This is the core of RAII.
   
    // Copying is deleted by default with std::unique_ptr, which is what we want.
    // The default move constructor and move assignment operator are used.
    // The compiler synthesizes these for us because the unique_ptr can be moved.


    void printMemory() const {
        // Access the raw pointer for printing, or use operator[] for elements.
        if (m_data) {
            std::cout << "Memory content: [";
            for (size_t i = 0; i < m_size; ++i) {
                std::cout << m_data[i] << (i == m_size - 1 ? "" : ", ");
            }
            std::cout << "]" << std::endl;
        } else {
            std::cout << "Memory content: (null) - object was moved from." << std::endl;
        }
    }


private:
    std::unique_ptr<int[]> m_data; // A unique_ptr to a dynamically allocated integer array.
    size_t m_size = 0;
};


void createAndDestroy() {
    std::cout << "--- Entering scope for DynamicMemoryHandler ---" << std::endl;
    // An object is created, and the constructor allocates memory.
    DynamicMemoryHandler myData(5);
    myData.printMemory();
    std::cout << "--- Exiting scope for DynamicMemoryHandler ---" << std::endl;
} // 'myData' goes out of scope here, and the unique_ptr's destructor is automatically called.


void demonstrateMoveSemantics() {
    std::cout << "\n--- Demonstrating Move Semantics with unique_ptr ---" << std::endl;
    DynamicMemoryHandler original(3);
   
    // The move constructor is called to transfer ownership.
    DynamicMemoryHandler moved_data = std::move(original);
   
    std::cout << "State of 'original' after move:" << std::endl;
    original.printMemory();


    std::cout << "State of 'moved_data' after move:" << std::endl;
    moved_data.printMemory();
    std::cout << "------------------------------------------" << std::endl;


} // 'original' and 'moved_data' destructors are called here.


int main() {
    createAndDestroy();
    demonstrateMoveSemantics();
    return 0;
}
