/*
g++ promises_futures_demo.cpp -o promises_futures_demo
./promises_futures_demo

Expected output:
Main: Calling get() - will block until shared state flag = READY
Worker: Starting work (shared state flag = NOT READY)
[pause]
Worker: About to set value (changing shared state flag to READY)
Worker: Value set, shared state flag now = READY
Received: worker_thread_1
Main: get() returned because shared state flag became READY
*/

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <chrono>

int main() {
    // 1. Create promise/future pair
    std::promise<std::string> example_promise_obj;
    std::future<std::string> example_future_obj = example_promise_obj.get_future();
    
    // 2. Spawn worker thread
    std::thread t1([&example_promise_obj]() {
        // 4. Worker thread starts executing (shared state flag = NOT READY)
        std::cout << "Worker: Starting work (shared state flag = NOT READY)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
        
        // 5. Worker thread prepares to set value
        std::string thread_id = "worker_thread_1";
        std::cout << "Worker: About to set value (changing shared state flag to READY)" << std::endl;
        
        // 6. Worker thread sets value (this changes shared state flag to READY)
        example_promise_obj.set_value(thread_id);
        std::cout << "Worker: Value set, shared state flag now = READY" << std::endl;
    });
    
    // 3. Main thread calls get() - will block until shared state flag = READY
    std::cout << "Main: Calling get() - will block until shared state flag = READY" << std::endl;
    
    // 7. Main thread gets the result (unblocks when shared state flag becomes READY)
    std::cout << "Received: " << example_future_obj.get() << std::endl;
    std::cout << "Main: get() returned because shared state flag became READY" << std::endl;
    
    // 8. Wait for worker thread to complete
    t1.join();
    
    return 0;
}
