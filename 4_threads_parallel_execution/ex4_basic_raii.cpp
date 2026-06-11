/* To run this program, use the following command:
g++ exercise_4_solution.cpp -o exercise_4
./exercise_4

This program demonstrates RAII (Resource Acquisition Is Initialization) using class destructors and smart pointers.

Expected output:
Starting main thread - RAII Smart Pointer Demo

=== Destructor Timing Example ===
Creating resources in inner scope...
CONSTRUCTOR: Resource 'ScopeTest1' created with value: 100
CONSTRUCTOR: Resource 'ScopeTest2' created with value: 200
Using resource 'ScopeTest1' with value: 100
Using resource 'ScopeTest2' with value: 200
About to exit inner scope...
DESTRUCTOR: Resource 'ScopeTest2' destroyed (value was: 200)
DESTRUCTOR: Resource 'ScopeTest1' destroyed (value was: 100)
Exited inner scope - destructors were called!

=== File Resource RAII Example ===
CONSTRUCTOR: File 'data.txt' opened
Writing to file 'data.txt': Hello World
Writing to file 'data.txt': RAII is awesome!
File operations complete, leaving scope...
DESTRUCTOR: File 'data.txt' closed automatically
File was automatically closed!
CONSTRUCTOR: Resource 'SharedResource' created with value: 999
Initial shared_ptr reference count: 1

=== Thread 1 - unique_ptr example ===

=== Thread 2 - shared_ptr example ===
Thread 2 - Reference count: 
=== Thread 3 - shared_ptr example ===
3CONSTRUCTOR: Resource '
Using resource 'SharedResource' with value: 999
UniqueRes1Using resource 'SharedResource' with value: 1001
Main thread reference count: Thread 3 - Reference count: 3
Using resource 'SharedResource' with value: 1001

=== Detached thread working... 1 ===
3
CONSTRUCTOR: Resource 'DetachedRes1' created with value: 100
Using resource 'DetachedRes1' with value: 100
' created with value: 10
Using resource 'UniqueRes1' with value: 10
Using resource 'UniqueRes1' with value: Using resource 'SharedResource' with value: 1004
15
Thread 1 - unique_ptr will auto-destroy when function ends
DESTRUCTOR: Resource 'UniqueRes1' destroyed (value was: 15)
Thread 2 - shared_ptr reference going out of scope
Thread 3 - shared_ptr reference going out of scope
After threads finished, reference count: 1
Main thread pausing for 3 seconds to show detached thread working...
DESTRUCTOR: Resource 'DetachedRes1' destroyed (value was: 100)

=== Detached thread working... 2 ===
CONSTRUCTOR: Resource 'DetachedRes2' created with value: 200
Using resource 'DetachedRes2' with value: 200
DESTRUCTOR: Resource 'DetachedRes2' destroyed (value was: 200)

=== Detached thread working... 3 ===
CONSTRUCTOR: Resource 'DetachedRes3' created with value: 300
Using resource 'DetachedRes3' with value: 300
DESTRUCTOR: Resource 'DetachedRes3' destroyed (value was: 300)
Main thread ending
shared_resource destructor will be called when main ends
DESTRUCTOR: Resource 'SharedResource' destroyed (value was: 1004)
*/

#include <iostream>
#include <thread>
#include <memory>
#include <chrono>

// Simple class to demonstrate RAII
class Resource {
public:
    int value;
    std::string name;
    
    Resource(int val, const std::string& n) : value(val), name(n) {
        std::cout << "CONSTRUCTOR: Resource '" << name << "' created with value: " << value << std::endl;
    }
    
    ~Resource() {
        std::cout << "DESTRUCTOR: Resource '" << name << "' destroyed (value was: " << value << ")" << std::endl;
        // This is where we would release any resources (close files, free memory, etc.)
    }
    
    void use() {
        std::cout << "Using resource '" << name << "' with value: " << value << std::endl;
    }
};

// More complex resource that manages a "file"
class FileResource {
private:
    std::string filename;
    bool is_open;
    
public:
    FileResource(const std::string& fname) : filename(fname), is_open(true) {
        std::cout << "CONSTRUCTOR: File '" << filename << "' opened" << std::endl;
    }
    
    ~FileResource() {
        if (is_open) {
            std::cout << "DESTRUCTOR: File '" << filename << "' closed automatically" << std::endl;
            // In real code, this would close the actual file
        }
    }
    
    void write_data(const std::string& data) {
        std::cout << "Writing to file '" << filename << "': " << data << std::endl;
    }
    
    void close() {
        if (is_open) {
            std::cout << "Manual close: File '" << filename << "' closed manually" << std::endl;
            is_open = false;
        }
    }
};

void unique_ptr_example(int thread_id)
{
    std::cout << "\n=== Thread " << thread_id << " - unique_ptr example ===" << std::endl;
    
    // unique_ptr: Exclusive ownership, automatically cleaned up
    std::unique_ptr<Resource> unique_resource = std::make_unique<Resource>(thread_id * 10, "UniqueRes" + std::to_string(thread_id));
    
    // Use the resource
    unique_resource->use();
    unique_resource->value += 5;
    unique_resource->use();
    
    std::cout << "Thread " << thread_id << " - unique_ptr will auto-destroy when function ends" << std::endl;
    
    // No need to manually delete - RAII handles it!
    // Resource destructor automatically called when unique_ptr goes out of scope
}

void shared_ptr_example(int thread_id, std::shared_ptr<Resource> shared_resource)
{
    std::cout << "\n=== Thread " << thread_id << " - shared_ptr example ===" << std::endl;
    std::cout << "Thread " << thread_id << " - Reference count: " << shared_resource.use_count() << std::endl;
    
    // Use the shared resource
    shared_resource->use();
    shared_resource->value += thread_id;
    shared_resource->use();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Thread " << thread_id << " - shared_ptr reference going out of scope" << std::endl;
    
    // shared_ptr automatically decrements reference count when destroyed
    // Resource destructor only called when last shared_ptr is destroyed
}

void destructor_timing_example()
{
    std::cout << "\n=== Destructor Timing Example ===" << std::endl;
    
    {
        std::cout << "Creating resources in inner scope..." << std::endl;
        Resource r1(100, "ScopeTest1");
        std::unique_ptr<Resource> r2 = std::make_unique<Resource>(200, "ScopeTest2");
        
        r1.use();
        r2->use();
        
        std::cout << "About to exit inner scope..." << std::endl;
    }   // <-- Destructors called here automatically!
    
    std::cout << "Exited inner scope - destructors were called!" << std::endl;
}

void file_resource_example()
{
    std::cout << "\n=== File Resource RAII Example ===" << std::endl;
    
    // Demonstrate automatic file cleanup
    {
        std::unique_ptr<FileResource> file = std::make_unique<FileResource>("data.txt");
        file->write_data("Hello World");
        file->write_data("RAII is awesome!");
        
        std::cout << "File operations complete, leaving scope..." << std::endl;
    }   // <-- File automatically closed by destructor!
    
    std::cout << "File was automatically closed!" << std::endl;
}

void detached_thread_function()
{
    int counter = 1;
    while (counter <= 3) {
        std::cout << "\n=== Detached thread working... " << counter << " ===" << std::endl;
        
        // Demonstrate unique_ptr in detached thread
        std::unique_ptr<Resource> temp_resource = std::make_unique<Resource>(counter * 100, "DetachedRes" + std::to_string(counter));
        temp_resource->use();
        // temp_resource destructor automatically called at end of each iteration
        
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        counter++;
    }
}

int main()
{
    std::cout << "Starting main thread - RAII Smart Pointer Demo" << std::endl;
    
    // Show destructor timing
    destructor_timing_example();
    
    // Show file resource example
    file_resource_example();
    
    // Create shared resource for shared_ptr examples
    std::shared_ptr<Resource> shared_resource = std::make_shared<Resource>(999, "SharedResource");
    std::cout << "Initial shared_ptr reference count: " << shared_resource.use_count() << std::endl;
    
    // Thread 1: unique_ptr example
    std::thread t1(unique_ptr_example, 1);
    
    // Thread 2: shared_ptr example
    std::thread t2(shared_ptr_example, 2, shared_resource);
    
    // Thread 3: shared_ptr example (same shared resource)
    std::thread t3(shared_ptr_example, 3, shared_resource);
    
    // Create and detach the 4th thread
    std::thread t4(detached_thread_function);
    t4.detach();
    
    std::cout << "Main thread reference count: " << shared_resource.use_count() << std::endl;
    
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << "After threads finished, reference count: " << shared_resource.use_count() << std::endl;
    
    std::cout << "Main thread pausing for 3 seconds to show detached thread working..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "Main thread ending" << std::endl;
    std::cout << "shared_resource destructor will be called when main ends" << std::endl;
    
    return 0;
    // shared_resource destructor automatically called here when main ends
}
