#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <optional>
#include <queue>
#include <iostream>
#include <utility>

#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <random>

// Compile and run with: 
// g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread enterprise_restaurant_queue.cpp -o demo && ./demo

template<typename OrderType>
class EnterpriseRestaurantQueue {
private:
    mutable std::mutex queue_coordination_mutex;
    std::condition_variable orders_available_signal;      // Kitchen staff waiting for work
    std::condition_variable queue_capacity_signal;        // Servers waiting to submit orders
    
    std::queue<OrderType> order_processing_queue;
    size_t maximum_queue_capacity;
    bool restaurant_operations_active = true;
    
    // Comprehensive operational metrics
    std::atomic<size_t> total_orders_submitted{0};
    std::atomic<size_t> total_orders_processed{0};
    std::atomic<size_t> orders_lost_to_capacity_limits{0};
    std::atomic<size_t> server_timeout_events{0};
    std::atomic<size_t> kitchen_timeout_events{0};
    std::atomic<size_t> peak_queue_depth{0};
    
public:
    explicit EnterpriseRestaurantQueue(size_t capacity) : maximum_queue_capacity(capacity) {}
    
    // Server interface - submitting orders with comprehensive error handling
    bool submit_customer_order(OrderType order, std::chrono::milliseconds submission_timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> coordination_lock(queue_coordination_mutex);
        
        // Wait for queue capacity with timeout handling
        if (!queue_capacity_signal.wait_for(coordination_lock, submission_timeout, [this] {
            return order_processing_queue.size() < maximum_queue_capacity || !restaurant_operations_active;
        })) {
            server_timeout_events.fetch_add(1);
            return false;  // Timeout - queue remained full
        }
        
        if (!restaurant_operations_active) {
            return false;  // Restaurant closed - not accepting orders
        }
        
        order_processing_queue.push(std::move(order));
        total_orders_submitted.fetch_add(1);
        
        // Update peak queue tracking
        size_t current_depth = order_processing_queue.size();
        size_t current_peak = peak_queue_depth.load();
        while (current_depth > current_peak && 
               !peak_queue_depth.compare_exchange_weak(current_peak, current_depth)) {
            // Atomic peak tracking with retry
        }
        
        orders_available_signal.notify_one();  // Alert kitchen staff
        return true;
    }
    
    // Kitchen interface - processing orders with comprehensive coordination
    std::optional<OrderType> retrieve_next_order(std::chrono::milliseconds retrieval_timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> coordination_lock(queue_coordination_mutex);
        
        // Wait for orders with timeout handling
        if (!orders_available_signal.wait_for(coordination_lock, retrieval_timeout, [this] {
            return !order_processing_queue.empty() || !restaurant_operations_active;
        })) {
            kitchen_timeout_events.fetch_add(1);
            return std::nullopt;  // Timeout - no orders became available
        }
        
        if (order_processing_queue.empty() && !restaurant_operations_active) {
            return std::nullopt;  // Restaurant closed and queue empty
        }
        
        OrderType next_order = std::move(order_processing_queue.front());
        order_processing_queue.pop();
        total_orders_processed.fetch_add(1);
        
        queue_capacity_signal.notify_one();  // Alert waiting servers
        return next_order;
    }
    
    // Non-blocking operations for high-performance scenarios
    bool try_submit_order_immediately(OrderType order) {
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        
        if (order_processing_queue.size() >= maximum_queue_capacity || !restaurant_operations_active) {
            orders_lost_to_capacity_limits.fetch_add(1);
            return false;
        }
        
        order_processing_queue.push(std::move(order));
        total_orders_submitted.fetch_add(1);
        orders_available_signal.notify_one();
        return true;
    }
    
    std::optional<OrderType> try_retrieve_order_immediately() {
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        
        if (order_processing_queue.empty()) {
            return std::nullopt;
        }
        
        OrderType immediate_order = std::move(order_processing_queue.front());
        order_processing_queue.pop();
        total_orders_processed.fetch_add(1);
        queue_capacity_signal.notify_one();
        return immediate_order;
    }
    
    // Restaurant management operations
    void cease_operations(bool process_remaining_orders = true) {
        {
            std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
            restaurant_operations_active = false;
            
            if (!process_remaining_orders) {
                // Immediate shutdown - clear pending orders
                std::queue<OrderType> empty_queue;
                order_processing_queue.swap(empty_queue);
            }
        }
        
        // Wake all waiting staff
        orders_available_signal.notify_all();
        queue_capacity_signal.notify_all();
    }
    
    // Comprehensive operational analytics
    struct RestaurantOperationalMetrics {
        size_t orders_submitted;
        size_t orders_processed;
        size_t orders_lost;
        size_t server_timeouts;
        size_t kitchen_timeouts;
        size_t current_queue_depth;
        size_t peak_queue_depth;
        double processing_efficiency;
        double capacity_utilization;
    };
    
    RestaurantOperationalMetrics generate_operational_report() const {
        size_t submitted = total_orders_submitted.load();
        size_t processed = total_orders_processed.load();
        size_t lost = orders_lost_to_capacity_limits.load();
        
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        size_t current_depth = order_processing_queue.size();
        size_t peak_depth = peak_queue_depth.load();
        
        return RestaurantOperationalMetrics{
            submitted,
            processed,
            lost,
            server_timeout_events.load(),
            kitchen_timeout_events.load(),
            current_depth,
            peak_depth,
            submitted > 0 ? static_cast<double>(processed) / submitted : 0.0,
            maximum_queue_capacity > 0 ? static_cast<double>(peak_depth) / maximum_queue_capacity : 0.0
        };
    }
    
    // Queue status queries
    size_t get_current_queue_depth() const {
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        return order_processing_queue.size();
    }
    
    bool is_accepting_orders() const {
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        return restaurant_operations_active;
    }
    
    bool has_pending_orders() const {
        std::lock_guard<std::mutex> coordination_lock(queue_coordination_mutex);
        return !order_processing_queue.empty();
    }
};

struct ComprehensiveOrder {
    int table_number;
    int server_id;
    std::string ordered_items;
    std::string special_dietary_requirements;
    std::chrono::steady_clock::time_point order_submission_time;
    double estimated_preparation_time_minutes;
    
    ComprehensiveOrder(int table, int server, const std::string& items, 
                      const std::string& requirements = "", double prep_time = 15.0)
        : table_number(table), server_id(server), ordered_items(items),
          special_dietary_requirements(requirements), 
          order_submission_time(std::chrono::steady_clock::now()),
          estimated_preparation_time_minutes(prep_time) {}
};

void demonstrate_complete_restaurant_operations() {
    const size_t restaurant_queue_capacity = 25;
    const int number_of_servers = 4;
    const int number_of_kitchen_staff = 3;
    const int orders_per_server = 15;
    
    EnterpriseRestaurantQueue<ComprehensiveOrder> restaurant_coordination_system(restaurant_queue_capacity);
    
    std::cout << "Launching comprehensive restaurant operation simulation..." << std::endl;
    std::cout << "Queue capacity: " << restaurant_queue_capacity << " orders" << std::endl;
    std::cout << "Staff: " << number_of_servers << " servers, " 
              << number_of_kitchen_staff << " kitchen staff" << std::endl;
    
    // Server staff (order producers)
    std::vector<std::thread> server_staff;
    for (int server_id = 0; server_id < number_of_servers; ++server_id) {
        server_staff.emplace_back([&restaurant_coordination_system, server_id, orders_per_server]() {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<> table_assignment(1, 30);
            std::uniform_real_distribution<> preparation_time(10.0, 45.0);
            
            std::vector<std::string> menu_items = {
                "Grilled Salmon with Vegetables", "Beef Tenderloin Steak", 
                "Chicken Caesar Salad", "Vegetarian Pasta Primavera",
                "Lobster Thermidor", "Duck Confit with Berry Sauce"
            };
            
            std::vector<std::string> dietary_requirements = {
                "", "Gluten-free", "Dairy-free", "Low-sodium", "Vegetarian"
            };
            
            for (int order_count = 0; order_count < orders_per_server; ++order_count) {
                int table = table_assignment(generator);
                std::string items = menu_items[order_count % menu_items.size()];
                std::string requirements = dietary_requirements[order_count % dietary_requirements.size()];
                double prep_time = preparation_time(generator);
                
                ComprehensiveOrder customer_order(table, server_id, items, requirements, prep_time);
                
                if (restaurant_coordination_system.submit_customer_order(
                    std::move(customer_order), std::chrono::milliseconds(300))) {
                    
                    std::cout << "Server " << server_id << " submitted order for table " 
                              << table << ": " << items << std::endl;
                } else {
                    std::cout << "Server " << server_id 
                              << " failed to submit order (timeout or restaurant closed)" << std::endl;
                }
                
                // Variable order submission timing
                std::this_thread::sleep_for(std::chrono::milliseconds(200 + (server_id * 50)));
            }
            
            std::cout << "Server " << server_id << " completed shift" << std::endl;
        });
    }
    
    // Kitchen staff (order processors)
    std::vector<std::thread> kitchen_staff;
    for (int cook_id = 0; cook_id < number_of_kitchen_staff; ++cook_id) {
        kitchen_staff.emplace_back([&restaurant_coordination_system, cook_id]() {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<> cooking_variation(50, 150);  // Percentage of estimated time
            
            while (true) {
                auto order = restaurant_coordination_system.retrieve_next_order(std::chrono::milliseconds(800));
                
                if (order.has_value()) {
                    auto current_time = std::chrono::steady_clock::now();
                    auto order_age = std::chrono::duration_cast<std::chrono::milliseconds>(
                        current_time - order->order_submission_time);
                    
                    // Calculate cooking time based on estimated preparation time with variation
                    int cooking_time_ms = static_cast<int>(
                        order->estimated_preparation_time_minutes  *1000*  
                        (cooking_variation(generator) / 100.0)
                    );
                    
                    std::cout << "Cook " << cook_id << " preparing " << order->ordered_items 
                              << " for table " << order->table_number 
                              << " (order age: " << order_age.count() << "ms)" << std::endl;
                    
                    // Simulate cooking process
                    std::this_thread::sleep_for(std::chrono::milliseconds(cooking_time_ms));
                    
                    std::cout << "Cook " << cook_id << " completed " << order->ordered_items 
                              << " for table " << order->table_number << std::endl;
                } else {
                    std::cout << "Cook " << cook_id << " timed out waiting for orders - ending shift" << std::endl;
                    break;
                }
            }
        });
    }
    
    // Restaurant manager (operational monitoring)
    std::thread restaurant_manager([&restaurant_coordination_system]() {
        for (int monitoring_cycle = 0; monitoring_cycle < 12; ++monitoring_cycle) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            auto operational_metrics = restaurant_coordination_system.generate_operational_report();
            
            std::cout << "\n=== Restaurant Manager Report (Cycle " << monitoring_cycle + 1 << ") ===" << std::endl;
            std::cout << "Orders Submitted: " << operational_metrics.orders_submitted << std::endl;
            std::cout << "Orders Processed: " << operational_metrics.orders_processed << std::endl;
            std::cout << "Orders Lost: " << operational_metrics.orders_lost << std::endl;
            std::cout << "Current Queue: " << operational_metrics.current_queue_depth << " orders" << std::endl;
            std::cout << "Peak Queue Depth: " << operational_metrics.peak_queue_depth << std::endl;
            std::cout << "Processing Efficiency: " << std::fixed << std::setprecision(1) 
                      << (operational_metrics.processing_efficiency * 100.0) << "%" << std::endl;
            std::cout << "Capacity Utilization: " << std::fixed << std::setprecision(1) 
                      << (operational_metrics.capacity_utilization * 100.0) << "%" << std::endl;
            std::cout << "Server Timeouts: " << operational_metrics.server_timeouts << std::endl;
            std::cout << "Kitchen Timeouts: " << operational_metrics.kitchen_timeouts << std::endl;
            std::cout << "========================================\n" << std::endl;
        }
    });
    
    // Operational simulation timeline
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    std::cout << "\nManager: Initiating restaurant closing procedures..." << std::endl;
    restaurant_coordination_system.cease_operations(true);  // Process remaining orders
    
    // Staff completion coordination
    for (auto& server : server_staff) {
        server.join();
    }
    
    for (auto& cook : kitchen_staff) {
        cook.join();
    }
    
    restaurant_manager.join();
    
    // Final comprehensive operational analysis
    auto final_operational_metrics = restaurant_coordination_system.generate_operational_report();
    std::cout << "\n=== Final Restaurant Operational Analysis ===" << std::endl;
    std::cout << "Total Orders Submitted: " << final_operational_metrics.orders_submitted << std::endl;
    std::cout << "Total Orders Processed: " << final_operational_metrics.orders_processed << std::endl;
    std::cout << "Orders Lost to Capacity: " << final_operational_metrics.orders_lost << std::endl;
    std::cout << "Orders Remaining in Queue: " << final_operational_metrics.current_queue_depth << std::endl;
    std::cout << "Peak Operational Load: " << final_operational_metrics.peak_queue_depth << " concurrent orders" << std::endl;
    std::cout << "Overall Processing Efficiency: " << std::fixed << std::setprecision(1) 
              << (final_operational_metrics.processing_efficiency * 100.0) << "%" << std::endl;
    std::cout << "Maximum Capacity Utilization: " << std::fixed << std::setprecision(1) 
              << (final_operational_metrics.capacity_utilization * 100.0) << "%" << std::endl;
    std::cout << "Total Server Timeout Events: " << final_operational_metrics.server_timeouts << std::endl;
    std::cout << "Total Kitchen Timeout Events: " << final_operational_metrics.kitchen_timeouts << std::endl;
    std::cout << "============================================" << std::endl;
}

int main() {
    demonstrate_complete_restaurant_operations();
    return 0;
}
