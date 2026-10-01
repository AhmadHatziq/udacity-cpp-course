/*
Exercise 2: Cache with Time-to-Live
Lesson 3, Section 4.3

This exercise demonstrates thread coordination using condition variables
to implement a cache that automatically expires data after a TTL period.

To compile and run:
g++ -std=c++11 -pthread exercise_2_solution.cpp -o exercise_2_solution && ./exercise_2_solution

Expected behavior:
- Multiple threads can store and retrieve cached values
- Items automatically expire after their TTL period
- Background cleanup thread removes expired items
- Threads can wait for specific keys to become available
*/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <vector>

class TTLCache {
private:
    struct CacheItem {
        int value;
        std::chrono::steady_clock::time_point expiry_time;
        
        CacheItem(int val, std::chrono::seconds ttl) 
            : value(val), expiry_time(std::chrono::steady_clock::now() + ttl) {}
    };
    
    std::unordered_map<std::string, CacheItem> cache;
    std::mutex cache_mutex; // Ensures only 1 thread at a time can access the hashmap 
    std::condition_variable key_available; // Allows consumers to sleep while waiting for a valid entry 
    bool cleanup_running;
    
public:
    TTLCache() : cleanup_running(true) {}
    
    // Store a value in the cache with TTL
    // Inserts an entry and wakes consumers up 
    void put(const std::string& key, int value, std::chrono::seconds ttl) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        cache.emplace(key, CacheItem(value, ttl));
        key_available.notify_all(); // Notify threads waiting for this key
        std::cout << "Stored key '" << key << "' with value " << value << std::endl;
    }
    
    // Get a value from cache (returns -1 if not found or expired)
    int get(const std::string& key) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        auto it = cache.find(key);
        
        if (it == cache.end()) {
            return -1; // Key not found
        }
        
        // Check if item has expired but exists 
        if (std::chrono::steady_clock::now() > it->second.expiry_time) {
            cache.erase(it);
            return -1; // Expired
        }
        
        return it->second.value;
    }
    
    // Wait for a key to become available (with timeout)
    // Sleeps until a key is valid or times out    
    bool wait_for_key(const std::string& key, std::chrono::seconds timeout) {
        std::unique_lock<std::mutex> lock(cache_mutex);
        
        auto deadline = std::chrono::steady_clock::now() + timeout;
        
        return key_available.wait_until(lock, deadline, [this, &key] {
            auto it = cache.find(key);
            if (it == cache.end()) return false;
            
            // Check if still valid
            return std::chrono::steady_clock::now() <= it->second.expiry_time;
        });
    }
    
    // Background cleanup of expired items
    void cleanup_expired() {
        while (cleanup_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::lock_guard<std::mutex> lock(cache_mutex);
            auto now = std::chrono::steady_clock::now();
            
            for (auto it = cache.begin(); it != cache.end();) {
                if (now > it->second.expiry_time) {
                    std::cout << "Cleaning up expired key: " << it->first << std::endl;
                    it = cache.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    
    void stop_cleanup() {
        cleanup_running = false;
    }
    
    // Get current cache size for demonstration
    size_t size() {
        std::lock_guard<std::mutex> lock(cache_mutex);
        return cache.size();
    }
};

// Thread function that stores values in cache
// For a single producer, insert 3 values 
void producer_thread(TTLCache& cache, int thread_id) {
    for (int i = 0; i < 3; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
        int value = thread_id * 100 + i;
        
        // Store with different TTL values
        std::chrono::seconds ttl(2 + i); // 2-4 seconds TTL
        cache.put(key, value, ttl);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Thread function that retrieves values from cache
// For a single consumer, only look for keys from thread 1 
void consumer_thread(TTLCache& cache, int thread_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let producer start
    
    for (int i = 0; i < 2; ++i) {
        std::string key = "key_1_" + std::to_string(i); // Look for keys from thread 1
        
        // Try to get the value
        int value = cache.get(key);
        if (value != -1) {
            std::cout << "Consumer " << thread_id << " found " << key << " = " << value << std::endl;
        } else {
            std::cout << "Consumer " << thread_id << " waiting for " << key << std::endl;
            
            // Wait for key to become available
            if (cache.wait_for_key(key, std::chrono::seconds(3))) {
                value = cache.get(key);
                std::cout << "Consumer " << thread_id << " got " << key << " = " << value << " after waiting" << std::endl;
            } else {
                std::cout << "Consumer " << thread_id << " timed out waiting for " << key << std::endl;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    TTLCache cache;
    
    std::cout << "Starting TTL Cache demonstration..." << std::endl;
    std::cout << "Items will expire after their TTL period" << std::endl;
    std::cout << std::endl;
    
    // Start cleanup thread
    std::thread cleanup_thread(&TTLCache::cleanup_expired, &cache);
    
    // Start producer threads. 3 producers 
    std::vector<std::thread> producers;
    for (int i = 1; i <= 2; ++i) {
        producers.emplace_back(producer_thread, std::ref(cache), i);
    }
    
    // Start consumer threads, 3 consumers 
    std::vector<std::thread> consumers;
    for (int i = 1; i <= 2; ++i) {
        consumers.emplace_back(consumer_thread, std::ref(cache), i);
    }
    
    // Wait for producers and consumers
    for (auto& t : producers) {
        t.join();
    }
    for (auto& t : consumers) {
        t.join();
    }
    
    std::cout << std::endl;
    std::cout << "Cache size before final cleanup: " << cache.size() << std::endl;
    
    // Wait a bit more to see cleanup in action
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "Cache size after cleanup: " << cache.size() << std::endl;
    
    // Stop cleanup thread
    cache.stop_cleanup();
    cleanup_thread.join();
    
    std::cout << "Exercise completed!" << std::endl;
    
    return 0;
} 
