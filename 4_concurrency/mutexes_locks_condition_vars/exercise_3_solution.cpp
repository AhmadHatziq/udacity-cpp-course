/*
Exercise 3: Multi-Consumer Producer Queue
Lesson 3, Section 5.3

This exercise demonstrates a complete producer-consumer system where one producer
generates work items and multiple consumers compete to process them efficiently.

To compile and run:
g++ -std=c++11 -pthread exercise_3_solution.cpp -o demo && ./demo

Expected behavior:
- One producer generates work items and adds to queue
- Multiple consumers compete to process work items
- Fair work distribution across consumers
- Graceful shutdown when production completes
*/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <vector>
#include <chrono>

class WorkQueue {
private:
    std::queue<int> work_items;
    std::mutex queue_mutex;
    std::condition_variable work_available;
    bool production_complete;
    
public:
    WorkQueue() : production_complete(false) {}
    
    // Producer adds work to the queue
    void add_work(int work_item) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        work_items.push(work_item);
        work_available.notify_one(); // Wake up one waiting consumer
        std::cout << "Producer added work item: " << work_item << std::endl;
    }
    
    // Consumer tries to get work from the queue
    bool get_work(int& work_item) {
        std::unique_lock<std::mutex> lock(queue_mutex); // Mutex prevents multiple consumers from taking the same object 
        
        // Wait until work is available or production is complete
        work_available.wait(lock, [this] {
            return !work_items.empty() || production_complete;
        });
        
        // Check if there's actual work to do
        if (work_items.empty()) {
            return false; // No more work and production is complete
        }
        
        // Get the work item
        work_item = work_items.front();
        work_items.pop();
        return true;
    }
    
    // Signal that production is complete
    void finish_production() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        production_complete = true;
        work_available.notify_all(); // Wake up all waiting consumers
        std::cout << "Producer finished - signaling all consumers" << std::endl;
    }
    
    // Get current queue size for monitoring
    size_t size() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return work_items.size();
    }
};

// Producer function - generates work items
void producer_function(WorkQueue& queue) {
    std::cout << "Producer starting..." << std::endl;
    
    // Generate 20 work items
    for (int i = 1; i <= 20; ++i) {
        queue.add_work(i);
        
        // Simulate variable production time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Signal that production is complete
    queue.finish_production();
    std::cout << "Producer completed all work generation" << std::endl;
}

// Consumer function - processes work items
void consumer_function(WorkQueue& queue, int consumer_id) {
    std::cout << "Consumer " << consumer_id << " starting..." << std::endl;
    
    int work_item;
    int items_processed = 0;
    
    // Keep trying to get work until none is available
    while (queue.get_work(work_item)) {
        std::cout << "Consumer " << consumer_id << " processing work item: " << work_item << std::endl;
        
        // Simulate work processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        std::cout << "Consumer " << consumer_id << " completed work item: " << work_item << std::endl;
        items_processed++;
    }
    
    std::cout << "Consumer " << consumer_id << " finished. Processed " 
              << items_processed << " items" << std::endl;
}

int main() {
    WorkQueue work_queue;
    const int num_consumers = 3;
    
    std::cout << "Starting Multi-Consumer Producer Queue demonstration" << std::endl;
    std::cout << "Producer will generate 20 work items" << std::endl;
    std::cout << "3 consumers will compete to process them" << std::endl;
    std::cout << std::endl;
    
    // Start the producer thread
    std::thread producer(producer_function, std::ref(work_queue));
    
    // Start multiple consumer threads
    std::vector<std::thread> consumers;
    for (int i = 1; i <= num_consumers; ++i) {
        consumers.emplace_back(consumer_function, std::ref(work_queue), i);
    }
    
    // Monitor queue size periodically
    std::thread monitor([&work_queue]() {
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "[Monitor] Current queue size: " << work_queue.size() << std::endl;
        }
    });
    
    // Wait for producer to complete
    producer.join();
    
    // Wait for all consumers to complete
    for (auto& consumer : consumers) {
        consumer.join();
    }
    
    // Stop monitoring
    monitor.join();
    
    std::cout << std::endl;
    std::cout << "Final queue size: " << work_queue.size() << std::endl;
    std::cout << "Exercise completed! All work items processed." << std::endl;
    
    return 0;
} 
