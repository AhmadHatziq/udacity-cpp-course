template<typename OrderType>
class RestaurantOrderQueue {
private:
    mutable std::mutex queue_mutex;
    std::condition_variable orders_available;    // For kitchen staff waiting for work
    std::condition_variable queue_capacity_available;  // For servers waiting to place orders
    
    std::queue<OrderType> order_queue;
    size_t maximum_queue_capacity;
    bool restaurant_closed = false;
    
    // Performance tracking
    std::atomic<size_t> total_orders_received{0};
    std::atomic<size_t> total_orders_completed{0};
    std::atomic<size_t> orders_rejected_due_to_capacity{0};
    
public:
    explicit RestaurantOrderQueue(size_t max_capacity) : maximum_queue_capacity(max_capacity) {}
    
    bool submit_order(OrderType order, std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        // Wait for queue capacity with timeout
        if (!queue_capacity_available.wait_for(lock, timeout, [this] {
            return order_queue.size() < maximum_queue_capacity || restaurant_closed;
        })) {
            orders_rejected_due_to_capacity.fetch_add(1);
            return false;  // Timeout - queue full too long
        }
        
        if (restaurant_closed) {
            return false;  // Not accepting new orders
        }
        
        order_queue.push(std::move(order));
        total_orders_received.fetch_add(1);
        
        orders_available.notify_one();  // Wake waiting kitchen staff
        return true;
    }
    
    std::optional<OrderType> get_next_order(std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        // Wait for orders with timeout
        if (!orders_available.wait_for(lock, timeout, [this] {
            return !order_queue.empty() || restaurant_closed;
        })) {
            return std::nullopt;  // Timeout - no orders available
        }
        
        if (order_queue.empty() && restaurant_closed) {
            return std::nullopt;  // Restaurant closed and no remaining orders
        }
        
        OrderType next_order = std::move(order_queue.front());
        order_queue.pop();
        total_orders_completed.fetch_add(1);
        
        queue_capacity_available.notify_one();  // Wake waiting servers
        return next_order;
    }
    
    void close_restaurant(bool complete_remaining_orders = true) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            restaurant_closed = true;
            
            if (!complete_remaining_orders) {
                // Clear queue for immediate shutdown
                std::queue<OrderType> empty_queue;
                order_queue.swap(empty_queue);
            }
        }
        
        // Wake all waiting threads
        orders_available.notify_all();
        queue_capacity_available.notify_all();
    }
    
    struct OperationalStats {
        size_t orders_received;
        size_t orders_completed;
        size_t orders_rejected;
        size_t current_queue_size;
        double completion_efficiency;
    };
    
    OperationalStats get_statistics() const {
        size_t received = total_orders_received.load();
        size_t completed = total_orders_completed.load();
        
        std::lock_guard<std::mutex> lock(queue_mutex);
        return OperationalStats{
            received,
            completed,
            orders_rejected_due_to_capacity.load(),
            order_queue.size(),
            received > 0 ? static_cast<double>(completed) / received : 0.0
        };
    }
};

#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <iostream>

struct RestaurantOrder {
    int table_number;
    std::string dish_name;
    std::string special_instructions;
    std::chrono::steady_clock::time_point order_time;
    
    RestaurantOrder(int table, const std::string& dish, const std::string& instructions = "")
        : table_number(table), dish_name(dish), special_instructions(instructions),
          order_time(std::chrono::steady_clock::now()) {}
};

void demonstrate_restaurant_operations() {
    RestaurantOrderQueue<RestaurantOrder> order_system(15);  // Queue capacity: 15 orders
    
    // Server threads (order producers)
    std::vector<std::thread> servers;
    for (int server_id = 0; server_id < 3; ++server_id) {
        servers.emplace_back([&order_system, server_id]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> table_dist(1, 20);
            std::vector<std::string> dishes = {"Caesar Salad", "Grilled Salmon", "Beef Stir Fry", "Chicken Parmesan"};
            
            for (int order_count = 0; order_count < 12; ++order_count) {
                int table = table_dist(gen);
                std::string dish = dishes[order_count % dishes.size()];
                
                RestaurantOrder new_order(table, dish, "Server " + std::to_string(server_id));
                
                if (order_system.submit_order(std::move(new_order), std::chrono::milliseconds(200))) {
                    std::cout << "Server " << server_id << " submitted order for table " 
                              << table << ": " << dish << std::endl;
                } else {
                    std::cout << "Server " << server_id << " failed to submit order (queue full or timeout)" << std::endl;
                }
                
                // Variable order submission rate
                std::this_thread::sleep_for(std::chrono::milliseconds(150 + (server_id * 50)));
            }
            
            std::cout << "Server " << server_id << " shift completed" << std::endl;
        });
    }
    
    // Kitchen staff threads (order consumers)
    std::vector<std::thread> kitchen_staff;
    for (int cook_id = 0; cook_id < 2; ++cook_id) {
        kitchen_staff.emplace_back([&order_system, cook_id]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> cooking_time_dist(200, 800);
            
            while (true) {
                auto order = order_system.get_next_order(std::chrono::milliseconds(500));
                
                if (order.has_value()) {
                    auto cooking_time = std::chrono::milliseconds(cooking_time_dist(gen));
                    auto wait_time = std::chrono::steady_clock::now() - order->order_time;
                    
                    std::cout << "Cook " << cook_id << " preparing " << order->dish_name 
                              << " for table " << order->table_number 
                              << " (waited " << std::chrono::duration_cast<std::chrono::milliseconds>(wait_time).count() 
                              << "ms)" << std::endl;
                    
                    // Simulate cooking time
                    std::this_thread::sleep_for(cooking_time);
                    
                    std::cout << "Cook " << cook_id << " completed " << order->dish_name 
                              << " for table " << order->table_number << std::endl;
                } else {
                    std::cout << "Cook " << cook_id << " timed out waiting for orders" << std::endl;
                    break;  // Exit on timeout
                }
            }
            
            std::cout << "Cook " << cook_id << " ending shift" << std::endl;
        });
    }
    
    // Restaurant manager monitoring thread
    std::thread manager([&order_system]() {
        for (int report = 0; report < 8; ++report) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            auto stats = order_system.get_statistics();
            std::cout << "=== Manager Report ===" << std::endl;
            std::cout << "Orders Received: " << stats.orders_received << std::endl;
            std::cout << "Orders Completed: " << stats.orders_completed << std::endl;
            std::cout << "Current Queue Size: " << stats.current_queue_size << std::endl;
            std::cout << "Orders Rejected: " << stats.orders_rejected << std::endl;
            std::cout << "Efficiency: " << std::fixed << std::setprecision(1) 
                      << (stats.completion_efficiency * 100.0) << "%" << std::endl;
            std::cout << "===================" << std::endl;
        }
    });
    
    // Let restaurant operate, then close gracefully
    std::this_thread::sleep_for(std::chrono::seconds(6));
    
    std::cout << "\nManager: Closing restaurant for the night..." << std::endl;
    order_system.close_restaurant(true);  // Complete remaining orders
    
    // Wait for all staff to finish
    for (auto& server : servers) {
        server.join();
    }
    
    for (auto& cook : kitchen_staff) {
        cook.join();
    }
    
    manager.join();
    
    // Final operational report
    auto final_stats = order_system.get_statistics();
    std::cout << "\n=== Final Restaurant Statistics ===" << std::endl;
    std::cout << "Total Orders Processed: " << final_stats.orders_completed << std::endl;
    std::cout << "Orders Remaining: " << final_stats.current_queue_size << std::endl;
    std::cout << "Final Efficiency: " << std::fixed << std::setprecision(1) 
              << (final_stats.completion_efficiency * 100.0) << "%" << std::endl;
}