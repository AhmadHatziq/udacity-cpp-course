#include <iostream>
#include <utility> // For std::move


// This class demonstrates RAII by managing a dynamically allocated integer array.
class DynamicMemoryHandler {
public:
    // Constructor: Acquires the resource (the memory block).
    DynamicMemoryHandler(size_t size) : m_size(size) {
        std::cout << "Constructor: Allocating a new integer array of size " << m_size << std::endl;
        m_data = new int[m_size];
        // Initialize the memory to a known state.
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = i + 1;
        }
        std::cout << "Memory allocated at address: " << m_data << std::endl;
    }


    // Destructor: Releases the resource (deletes the memory block).
    // This is the core of RAII. It's called automatically when the object goes out of scope.
    ~DynamicMemoryHandler() {
        if (m_data != nullptr) {
            std::cout << "Destructor: Freeing memory at address " << m_data << std::endl;
            delete[] m_data;
        } else {
            std::cout << "Destructor: No memory to free (object was moved from)." << std::endl;
        }
    }


    // Copying is dangerous for classes with dynamic memory, so we delete it.
    DynamicMemoryHandler(const DynamicMemoryHandler&) = delete;
    DynamicMemoryHandler& operator=(const DynamicMemoryHandler&) = delete;


    // Move Constructor: Efficiently "steals" the resource from another object.
    DynamicMemoryHandler(DynamicMemoryHandler&& other) noexcept
        : m_data(other.m_data), m_size(other.m_size) {
        std::cout << "Move Constructor: Stealing memory from " << other.m_data << std::endl;
        // Reset the source object to a valid, empty state.
        other.m_data = nullptr;
        other.m_size = 0;
    }


    // Move Assignment Operator: Steals the resource from another object.
    DynamicMemoryHandler& operator=(DynamicMemoryHandler&& other) noexcept {
        std::cout << "Move Assignment: " << std::endl;
        if (this != &other) {
            // First, free our own existing memory.
            delete[] m_data;
            // Then, steal the resource from the source.
            m_data = other.m_data;
            m_size = other.m_size;
            // Reset the source object to a valid, empty state.
            other.m_data = nullptr;
            other.m_size = 0;
        }
        return *this;
    }


    void printMemory() const {
        if (m_data != nullptr) {
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
    int* m_data = nullptr;
    size_t m_size = 0;
};


void createAndDestroy() {
    std::cout << "--- Entering scope for DynamicMemoryHandler ---" << std::endl;
    // An object is created, and the constructor allocates memory.
    DynamicMemoryHandler myData(5);
    myData.printMemory();
    std::cout << "--- Exiting scope for DynamicMemoryHandler ---" << std::endl;
} // 'myData' goes out of scope here, and the destructor is automatically called.


void demonstrateMoveSemantics() {
    std::cout << "\n--- Demonstrating Move Semantics ---" << std::endl;
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
