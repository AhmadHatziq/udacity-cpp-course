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
    
    // Visual status bar on main thread
    while (example_future_obj.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        int current_progress = progress.load();
        
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
