/*To run this program, use the following command:
g++ exercise_2_solution.cpp -o exercise_2
./exercise_2

Expected output (the progress bar's values will differ as its continuously updated):
Progress: [                    ] 0%
Progress: [==                ] 20%
Progress: [====              ] 40%
Progress: [======            ] 60%
Progress: [========          ] 80%
Progress: [==================] 100%
Work completed!
*/


#include <iostream>
#include <future>
#include <chrono>
#include <atomic>
#include <thread>

int main() {
    // Shared progress counter. 'progress' is atomic int 
    std::atomic<int> progress = 0;
    
    // There are 2 threads: main & background 
    // Background work with std::launch::async. Takes i `progress` as a shared variable 
    // Shared state: Background thread writes to `progress`. Main thread reads from `progress`
    std::future<std::string> example_future_obj = std::async(std::launch::async, [&progress]() { 
    // Starts async ie immediately in another thread 
    // Product of this thread is the future var called 'example_future_obj'
        for (int i = 0; i <= 100; ++i) {
            // Simulate work in steps of 50ms 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            progress = i;  // Update progress
        }

        // Give main thread time to display 100% before finishing
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return std::string("Work completed!");
    });
    
    // Visual status bar on main thread
    // Keep looping when the async process is not in "ready" state 
    // While loop will keep polling the state of the thread until "ready" 
    while (example_future_obj.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        int current_progress = progress.load(); // Read atomic value (updated via background thread)
        
        // Simple text-based progress bar based on value of 'current_progress'
        std::cout << "\rProgress: ["; // Uses carriage return "\r", which brings cursor back to the start of the line 
        for (int i = 0; i < 20; ++i) {
            if (i < current_progress / 5) {
                std::cout << "=";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << current_progress << "%" << std::flush; // Use flush as std::cout may buffer output rather than immediately displaying it
    }
    
    std::string result = example_future_obj.get(); // Final value of the promise is string: "Work completed!"
    std::cout << "\n" << result << std::endl;
    
    return 0;
}
