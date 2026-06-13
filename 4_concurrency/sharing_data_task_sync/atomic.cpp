/*To run this program, use the following command:
g++ atomic.cpp -o exercise 
./exercise

Expected output (the progress bar's values will differ as its continuously updated):
Progress: [                    ] 0%
Progress: [==                ] 20%
Progress: [====              ] 40%
Progress: [======            ] 60%
Progress: [========          ] 80%
Progress: [==================] 100%
Work completed!

Code needs to be atomic as there is an update and reading happening at the same time via: 
progress = i;  // Update progress by worker thread 
int current_progress = progress.load(); // Main thread reads the progress safely (progress is atomic)
*/


#include <iostream>
#include <future>
#include <chrono>
#include <atomic>
#include <thread>

int main() {
    // Shared progress counter
    std::atomic<int> progress = 0;
    
    // Background work with std::launch::async
    std::future<std::string> example_future_obj = std::async(std::launch::async, [&progress]() {
        for (int i = 0; i <= 100; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            progress = i;  // Update progress
        }
        // Give main thread time to display 100% before finishing
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return std::string("Work completed!");
    });
    
    // Visual status bar on main thread. If the future is ready, loop stops. 
    // We do not use future.get() here because it would block until the async task is complete, preventing us from reading the progress updates in real-time. 
    // Instead, we use wait_for with a timeout to periodically check if the future is ready while still allowing us to read and display the progress.
    while (example_future_obj.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        int current_progress = progress.load(); // Main thread reads the progress safely (progress is atomic)
        
        // Simple text-based progress bar
        std::cout << "\rProgress: [";
        for (int i = 0; i < 20; ++i) {
            if (i < current_progress / 5) {
                std::cout << "=";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << current_progress << "%" << std::flush;
    }
    
    std::string result = example_future_obj.get();
    std::cout << "\n" << result << std::endl;
    
    return 0;
}
