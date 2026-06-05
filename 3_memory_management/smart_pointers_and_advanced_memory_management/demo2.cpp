#include <iostream>
#include <memory> // For std::shared_ptr and std::weak_ptr
#include <vector>


// A simple class to demonstrate object creation and destruction.
class MyObject {
public:
    MyObject(int value) : m_value(value) {
        std::cout << "  MyObject created with value " << m_value << std::endl;
    }
    ~MyObject() {
        std::cout << "  MyObject destroyed with value " << m_value << std::endl;
    }
    int getValue() const { return m_value; }
private:
    int m_value;
};


// --- Demo of std::shared_ptr ---
void sharedPointerDemo() {
    std::cout << "--- Shared Pointer Demo ---" << std::endl;
    std::cout << "Entering sharedPointerDemo()" << std::endl;


    // Create a shared_ptr. The reference count is 1.
    std::shared_ptr<MyObject> ptr1 = std::make_shared<MyObject>(100);
    std::cout << "  ptr1 created. Ref count: " << ptr1.use_count() << std::endl;


    // Create a new shared_ptr that shares ownership with ptr1.
    // The reference count is now 2.
    std::shared_ptr<MyObject> ptr2 = ptr1;
    std::cout << "  ptr2 created as a copy of ptr1. Ref count: " << ptr1.use_count() << std::endl;


    // ptr2 goes out of scope here, decrementing the reference count to 1.
    std::cout << "Exiting sharedPointerDemo()" << std::endl;
} // When ptr1 goes out of scope here, the reference count becomes 0, and MyObject is destroyed.


// --- Demo of Circular Reference (Memory Leak) ---
// Two classes with shared_ptrs pointing to each other.
class NodeB;
class NodeA {
public:
    std::shared_ptr<NodeB> ptrB;
    ~NodeA() { std::cout << "  NodeA destroyed." << std::endl; }
};


class NodeB {
public:
    std::shared_ptr<NodeA> ptrA;
    ~NodeB() { std::cout << "  NodeB destroyed." << std::endl; }
};


void circularReferenceDemo() {
    std::cout << "\n--- Circular Reference Demo (Memory Leak) ---" << std::endl;
    std::cout << "Entering circularReferenceDemo()" << std::endl;
    std::shared_ptr<NodeA> a = std::make_shared<NodeA>();
    std::shared_ptr<NodeB> b = std::make_shared<NodeB>();


    // Create a circular reference.
    // a's shared_ptr to b increments b's ref count to 2.
    // b's shared_ptr to a increments a's ref count to 2.
    a->ptrB = b;
    b->ptrA = a;


    std::cout << "  NodeA ref count: " << a.use_count() << std::endl;
    std::cout << "  NodeB ref count: " << b.use_count() << std::endl;
    std::cout << "Exiting circularReferenceDemo()" << std::endl;
} // a and b go out of scope, but their reference counts are still 1, so memory is leaked.


// --- Demo of std::weak_ptr (Fixing the Leak) ---
// The same two classes, but one uses a weak_ptr.
class NodeD;
class NodeC {
public:
    std::shared_ptr<NodeD> ptrD;
    ~NodeC() { std::cout << "  NodeC destroyed." << std::endl; }
};


class NodeD {
public:
    // This is the key change: a weak_ptr does not increase the reference count.
    std::weak_ptr<NodeC> ptrC;
    ~NodeD() { std::cout << "  NodeD destroyed." << std::endl; }
};


void weakPointerDemo() {
    std::cout << "\n--- Weak Pointer Demo (Fixing the Leak) ---" << std::endl;
    std::cout << "Entering weakPointerDemo()" << std::endl;
    std::shared_ptr<NodeC> c = std::make_shared<NodeC>();
    std::shared_ptr<NodeD> d = std::make_shared<NodeD>();


    // This creates the link, but only the shared_ptr from NodeC to NodeD
    // increases the reference count.
    c->ptrD = d;
    d->ptrC = c; // This is a weak pointer, it does not increase c's ref count.


    std::cout << "  NodeC ref count: " << c.use_count() << std::endl;
    std::cout << "  NodeD ref count: " << d.use_count() << std::endl;


    // A weak_ptr cannot be dereferenced directly. You must first lock it
    // to get a shared_ptr, which may or may not succeed.
    if (auto sharedC = d->ptrC.lock()) {
        std::cout << "  Weak pointer successfully locked to a shared_ptr. The value is still valid." << std::endl;
    } else {
        std::cout << "  Weak pointer is expired, the object is gone." << std::endl;
    }


    std::cout << "Exiting weakPointerDemo()" << std::endl;
} // When c and d go out of scope, their shared_ptrs are destroyed, ref counts drop to 0, and memory is freed.


int main() {
    sharedPointerDemo();
    circularReferenceDemo(); // This will leak memory.
    weakPointerDemo();       // This will not leak memory.
    return 0;
}
