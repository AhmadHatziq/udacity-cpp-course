/*
Exercise 1: Print Synchronization with Mutexes
Lesson 3, Section 3.4

This exercise demonstrates how to use mutexes to coordinate thread execution
order, ensuring multiple threads print in a predetermined sequence regardless
of thread scheduling or timing.

To compile and run:
g++ exercise_1_solution.cpp -o running_exercise  -pthread && ./running_exercise

Expected behavior:
- Threads finish preparation at different times
- But announcements happen in predetermined order (1, 2, 3, 4)
- Later threads wait for earlier threads even if ready first
*/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <chrono>

class OrderedPrinter {
private:
    std::mutex coordination_mutex;
    std::condition_variable turn_signal;
    int next_thread_to_execute;
    
public:
    OrderedPrinter() : next_thread_to_execute(1) {}
    
    void ordered_print(int thread_id, const std::string& location_name) {
        std::unique_lock<std::mutex> lock(coordination_mutex); // RAII unique lock 
        
        // Wait for this thread's turn to execute
        // Use condition variable, predicate is when the next_thread_to_execute var matches arg thread_id 
        turn_signal.wait(lock, [this, thread_id] {
            return next_thread_to_execute == thread_id;
        });
        
        // Once predicate matches, will proceed with execution of below code. 

        // This thread's turn - execute the printing sequence
        std::cout << "=== " << location_name << " Restaurant Announcement ===" << std::endl;
        std::cout << "Thread " << thread_id << " (" << location_name << ") starting daily specials announcement..." << std::endl;
        
        // Simulate announcement time (while holding coordination lock)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "Thread " << thread_id << ": Today's special is Grilled Salmon with herbs!" << std::endl;
        std::cout << "Thread " << thread_id << ": Fresh ingredients sourced this morning!" << std::endl;
        std::cout << "Thread " << thread_id << ": Available until supplies last!" << std::endl;
        std::cout << "Thread " << thread_id << " (" << location_name << ") completed announcement." << std::endl;
        std::cout << std::endl;
        
        // Signal that the next thread can proceed
        next_thread_to_execute++; // Starts from 1. Execution order is increasing order
        turn_signal.notify_all(); // Asks all condition vars to check predicate. If predicate okay, proceed for that thread. 
    }
};

// Restaurant thread function
void restaurant_announcement(OrderedPrinter& printer, int thread_id, const std::string& location) {
    // Simulate different preparation times - later threads finish prep first!
    int prep_time = 200 * (5 - thread_id); // Thread 4 finishes first, Thread 1 last
    std::this_thread::sleep_for(std::chrono::milliseconds(prep_time));
    
    std::cout << "[Background] Thread " << thread_id << " (" << location 
              << ") finished preparation, waiting for turn..." << std::endl;
    
    // Execute in coordinated order (not preparation order)
    printer.ordered_print(thread_id, location);
    
    std::cout << "[Background] Thread " << thread_id << " (" << location 
              << ") completed and ready for customers!" << std::endl;
}

int main() {
    OrderedPrinter printer;
    const int num_locations = 4;
    
    std::cout << "=== Carmen's Restaurant Chain Daily Specials Coordination ===" << std::endl;
    std::cout << "Multiple locations must announce specials in predetermined order" << std::endl;
    std::cout << "for brand consistency, regardless of preparation timing." << std::endl;
    std::cout << "Notice: Later threads will finish prep first but wait their turn!" << std::endl;
    std::cout << std::endl;
    
    // Restaurant locations
    std::vector<std::string> locations = {
        "Downtown", "Uptown", "Midtown", "Riverside"
    };
    
    std::vector<std::thread> restaurant_threads;
    
    // Create threads (they will finish preparation at different times)
    std::cout << "Starting all restaurant locations simultaneously..." << std::endl;
    for (int i = 1; i <= num_locations; ++i) {

        // Each call to emplace_back() constructs a std::thread directly inside the vector
        // and that thread starts running as soon as its std::thread object is successfully constructed.
        restaurant_threads.emplace_back(
            restaurant_announcement, 
            std::ref(printer), 
            i, 
            locations[i-1]
        );

        // Thread 1 → restaurant_announcement(printer, 1, "Downtown") 
    }
    
    // Wait for all restaurants to complete their coordinated announcements
    for (auto& t : restaurant_threads) {
        t.join();
    }
    
    std::cout << "=== All restaurant locations completed announcements in proper order! ===" << std::endl;
    std::cout << "Brand consistency maintained through mutex-based coordination." << std::endl;
    std::cout << std::endl;
    std::cout << "Key Points Demonstrated:" << std::endl;
    std::cout << "1. Threads executed in predetermined order (1->2->3->4)" << std::endl;
    std::cout << "2. Order enforced by mutex + condition variable coordination" << std::endl;
    std::cout << "3. Later threads waited even when ready first" << std::endl;
    std::cout << "4. Mutex controlled execution timing, not just data protection" << std::endl;
    
    return 0;
}
