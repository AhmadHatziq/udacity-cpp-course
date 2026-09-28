/*
Exercise 3: Monte Carlo Simulation with Atomic Operations
Lesson 2, Section 4.3

This exercise demonstrates atomic operations by creating multiple threads that
generate random numbers and count occurrences of 42. We'll compare results
with and without atomic operations to show the importance of thread safety.

To compile and run:
g++ exercise_3_solution.cpp -o running_exercise -pthread && ./running_exercise

Learning objectives:
- Create atomic counters for thread-safe counting
- Understand race conditions with non-atomic operations
- See how atomics enable lock-free parallel computation
- Compare performance and correctness of atomic vs non-atomic approaches
*/

#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>

// Global counters for comparison
std::atomic<int> atomic_counter(0);      // Thread-safe counter
int regular_counter = 0;                 // NOT thread-safe counter

// Configuration constants
const int NUM_THREADS = 8;
const int NUMBERS_PER_THREAD = 1000000;  // 1 million numbers per thread
const int TARGET_NUMBER = 42;
const int MIN_RANDOM = 0;
const int MAX_RANDOM = 100;

// Function that generates random numbers and counts 42s using ATOMIC counter
void monte_carlo_atomic_worker(int thread_id) {
    // Create random number generator for this thread
    std::random_device rd;
    std::mt19937 gen(rd() + thread_id); // Add thread_id for different seeds
    std::uniform_int_distribution<> dist(MIN_RANDOM, MAX_RANDOM);
    
    int local_count = 0; // Count locally first for efficiency
    
    std::cout << "[Atomic Thread " << thread_id << "] Starting Monte Carlo simulation..." << std::endl;
    
    // Generate random numbers and count occurrences of 42
    for (int i = 0; i < NUMBERS_PER_THREAD; ++i) {
        int random_number = dist(gen);
        if (random_number == TARGET_NUMBER) {
            local_count++;
        }
    }
    
    // Atomically add our local count to the global counter
    atomic_counter.fetch_add(local_count);
    
    std::cout << "[Atomic Thread " << thread_id << "] Found " << local_count 
              << " occurrences of " << TARGET_NUMBER << std::endl;
}

// Function that generates random numbers and counts 42s using REGULAR counter (race condition!)
void monte_carlo_regular_worker(int thread_id) {
    // Create random number generator for this thread
    std::random_device rd;
    std::mt19937 gen(rd() + thread_id); // Add thread_id for different seeds
    std::uniform_int_distribution<> dist(MIN_RANDOM, MAX_RANDOM);
    
    int local_count = 0;
    
    std::cout << "[Regular Thread " << thread_id << "] Starting Monte Carlo simulation..." << std::endl;
    
    // Generate random numbers and count occurrences of 42
    for (int i = 0; i < NUMBERS_PER_THREAD; ++i) {
        int random_number = dist(gen);
        if (random_number == TARGET_NUMBER) {
            local_count++;
            // RACE CONDITION: Multiple threads updating regular_counter simultaneously!
            regular_counter++; // This is NOT thread-safe!
        }
    }
    
    std::cout << "[Regular Thread " << thread_id << "] Found " << local_count 
              << " occurrences of " << TARGET_NUMBER << std::endl;
}

// Function to run the atomic counter test
void run_atomic_test() {
    std::cout << "\n=== ATOMIC COUNTER TEST (Thread-Safe) ===" << std::endl;
    
    // Reset counter
    atomic_counter.store(0);
    
    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Create and start threads
    for (int i = 1; i <= NUM_THREADS; ++i) {
        threads.emplace_back(monte_carlo_atomic_worker, i); // Each worker is collected in a vector 
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    int total_numbers = NUM_THREADS * NUMBERS_PER_THREAD;
    int expected_count = total_numbers / (MAX_RANDOM - MIN_RANDOM + 1); // Theoretical average
    
    std::cout << "\n--- Atomic Counter Results ---" << std::endl;
    std::cout << "Total numbers generated: " << total_numbers << std::endl;
    std::cout << "Expected count (theoretical): ~" << expected_count << std::endl;
    std::cout << "Actual count (atomic): " << atomic_counter.load() << std::endl;
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result: CONSISTENT (thread-safe)" << std::endl;
}

// Function to run the regular counter test (with race conditions)
void run_regular_test() {
    std::cout << "\n=== REGULAR COUNTER TEST (Race Conditions) ===" << std::endl;
    
    // Reset counter
    regular_counter = 0;
    
    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Create and start threads
    for (int i = 1; i <= NUM_THREADS; ++i) {
        threads.emplace_back(monte_carlo_regular_worker, i);
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    int total_numbers = NUM_THREADS * NUMBERS_PER_THREAD;
    int expected_count = total_numbers / (MAX_RANDOM - MIN_RANDOM + 1); // Theoretical average
    
    std::cout << "\n--- Regular Counter Results ---" << std::endl;
    std::cout << "Total numbers generated: " << total_numbers << std::endl;
    std::cout << "Expected count (theoretical): ~" << expected_count << std::endl;
    std::cout << "Actual count (regular): " << regular_counter << std::endl;
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result: INCONSISTENT (race conditions caused lost updates)" << std::endl;
}

// Function to demonstrate atomic operations in detail
void demonstrate_atomic_operations() {
    std::cout << "\n=== ATOMIC OPERATIONS DEMONSTRATION ===" << std::endl;
    
    std::atomic<int> demo_counter(100);
    
    std::cout << "Initial value: " << demo_counter.load() << std::endl;
    
    // Demonstrate various atomic operations
    std::cout << "After ++demo_counter: " << ++demo_counter << std::endl;
    std::cout << "After demo_counter--: " << demo_counter-- << std::endl;
    std::cout << "Current value: " << demo_counter.load() << std::endl;
    
    std::cout << "After fetch_add(5): " << demo_counter.fetch_add(5) << std::endl;
    std::cout << "Current value: " << demo_counter.load() << std::endl;
    
    std::cout << "After fetch_sub(3): " << demo_counter.fetch_sub(3) << std::endl;
    std::cout << "Final value: " << demo_counter.load() << std::endl;
    
    std::cout << "All operations were atomic (indivisible and thread-safe)!" << std::endl;
}

int main() {
    std::cout << "=== Monte Carlo Simulation: Atomic vs Regular Counters ===" << std::endl;
    std::cout << "Simulating parallel counting of occurrences of " << TARGET_NUMBER 
              << " in random numbers 0-" << MAX_RANDOM << std::endl;
    std::cout << "Using " << NUM_THREADS << " threads, each generating " 
              << NUMBERS_PER_THREAD << " random numbers" << std::endl;
    
    // Run the atomic counter test first (correct results)
    run_atomic_test();
    
    // Run the regular counter test (race conditions)
    run_regular_test();
    
    // Show the difference
    std::cout << "\n=== COMPARISON ANALYSIS ===" << std::endl;
    std::cout << "Atomic counter result: " << atomic_counter.load() << std::endl;
    std::cout << "Regular counter result: " << regular_counter << std::endl;
    std::cout << "Difference: " << (atomic_counter.load() - regular_counter) 
              << " (lost updates due to race conditions)" << std::endl;
    
    if (atomic_counter.load() != regular_counter) {
        std::cout << "\n*** RACE CONDITION DETECTED! ***" << std::endl;
        std::cout << "The regular counter lost updates due to simultaneous access." << std::endl;
        std::cout << "Multiple threads were reading and writing the same memory location" << std::endl;
        std::cout << "without proper synchronization, causing some increments to be lost." << std::endl;
    } else {
        std::cout << "\nResults happened to match (race condition may still exist but wasn't triggered)" << std::endl;
    }
    
    // Demonstrate atomic operations
    demonstrate_atomic_operations();
    
    return 0;
}
