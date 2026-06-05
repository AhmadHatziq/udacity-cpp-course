#include <iostream>
#include <cstddef>
#include <vector>

// Define a simple object we will allocate from our memory pool.
class MyObject {
public:
    MyObject(int id) : m_id(id) {
        std::cout << "  MyObject constructor called for ID " << m_id << std::endl;
    }

    ~MyObject() {
        std::cout << "  MyObject destructor called for ID " << m_id << std::endl;
    }

    void print() const {
        std::cout << "  MyObject at address " << this << " with ID " << m_id << std::endl;
    }
private:
    int m_id;
};

// Use a struct to guarantee that the `next` pointer is at the beginning of the block.
// The block size must be large enough to hold the object.
struct FreeBlock {
    FreeBlock* next;
};

// The MemoryPool class
class MemoryPool {
public:
    // Constructor: Pre-allocates a large block of memory.
    MemoryPool(size_t object_count) : m_object_count(object_count) {
        // The size of each memory block should be the max of the object size and the FreeBlock size.
        // This ensures the block is large enough for both a FreeBlock pointer and the object.
        const size_t block_size = std::max(sizeof(MyObject), sizeof(FreeBlock));
        m_pool_start = new char[m_object_count * block_size];
        m_free_list = nullptr;

        // Link all the blocks into a free list.
        for (size_t i = 0; i < m_object_count; ++i) {
            FreeBlock* block = reinterpret_cast<FreeBlock*>(m_pool_start + i * block_size);
            block->next = m_free_list;
            m_free_list = block;
        }

        std::cout << "MemoryPool created. Pre-allocated " << m_object_count * block_size << " bytes." << std::endl;
    }

    // Destructor: Frees the entire pre-allocated block of memory.
    ~MemoryPool() {
        std::cout << "MemoryPool destroyed. Freeing entire memory block." << std::endl;
        delete[] m_pool_start;
    }

    // Allocation function
    void* allocate() {
        if (m_free_list == nullptr) {
            std::cout << "Error: Memory pool is full." << std::endl;
            return nullptr;
        }

        // Take the first block from the free list.
        void* allocated_block = m_free_list;
        m_free_list = m_free_list->next;
        std::cout << "  Allocated block at " << allocated_block << std::endl;
        return allocated_block;
    }

    // Deallocation function
    void deallocate(void* ptr) {
        if (ptr == nullptr) {
            return;
        }

        std::cout << "  Deallocating block at " << ptr << std::endl;
        // Cast the memory back to a FreeBlock pointer.
        FreeBlock* new_free_block = reinterpret_cast<FreeBlock*>(ptr);
        // Add it to the front of the free list.
        new_free_block->next = m_free_list;
        m_free_list = new_free_block;
    }

private:
    char* m_pool_start;
    FreeBlock* m_free_list;
    size_t m_object_count;
};

int main() {
    // Create a memory pool that can hold 5 MyObject instances.
    MemoryPool pool(5);

    // Use a vector to store our pointers. We will manually call destructors.
    std::vector<MyObject*> objects;
    
    std::cout << "\n--- Allocating objects from the pool ---" << std::endl;
    for (int i = 0; i < 5; ++i) {
        // Allocate a block of memory and then use placement new to construct the object.
        void* memory = pool.allocate();
        if (memory) {
            objects.push_back(new (memory) MyObject(i));
        }
    }

    std::cout << "\n--- Deallocating some objects ---" << std::endl;
    // Explicitly call the destructor and then deallocate the memory back to the pool.
    objects[2]->~MyObject();
    pool.deallocate(objects[2]);
    objects[3]->~MyObject();
    pool.deallocate(objects[3]);
    objects.erase(objects.begin() + 2, objects.begin() + 4);

    std::cout << "\n--- Allocating new objects (should reuse old memory) ---" << std::endl;
    // The new objects should reuse the memory that was just deallocated.
    void* memory = pool.allocate();
    if (memory) {
        objects.push_back(new (memory) MyObject(100));
    }
    memory = pool.allocate();
    if (memory) {
        objects.push_back(new (memory) MyObject(101));
    }

    std::cout << "\n--- Final state of objects ---" << std::endl;
    for (const auto& obj : objects) {
        obj->print();
    }

    // Call destructors for remaining objects before the pool is destroyed.
    std::cout << "\n--- Calling destructors for remaining objects ---" << std::endl;
    for (const auto& obj : objects) {
        obj->~MyObject();
    }

    std::cout << "\nEnd of main function. The pool will now be destroyed." << std::endl;
    return 0;
}