/*
To run this program, use the following command:
g++ demo_unsafe_variable_lifetime.cpp -o demo_unsafe_variable_lifetime
./demo_unsafe_variable_lifetime

*/

#include <iostream>
#include <thread>
#include <chrono>
#include <string>


void demonstrate_unsafe_lifetime()
{
    std::cout << "\n=== Demonstrating UNSAFE Variable Lifetime ===" << std::endl;
    
    std::thread t;
    {
        std::string local_data = "Data that will be destroyed too early";
        
        // Create thread but DON'T join immediately
        t = std::thread([](const std::string& data) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay
            std::cout << "Accessing potentially destroyed data: " << data << std::endl;
        }, std::ref(local_data));
        
        // In real unsafe code, we might detach or not join here
        // For safety in this demo, we'll still join but show the concept
        std::cout << "Scope ending while thread is still running..." << std::endl;
    }
    // In unsafe code, local_data would be destroyed here while thread is still running
    t.join(); // Join for safety in this demo
}

int main()
{ 
    demonstrate_unsafe_lifetime();     
    return 0;
}
