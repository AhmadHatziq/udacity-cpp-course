#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct TomatoBatch {
    int batch_id;
    std::string variety;
    int quantity;
};

template <typename IngredientType>
class IngredientRequestSystem {
private:
    std::mutex request_mutex; // Mutex ensures only 1 object can edit the queue at a time. 
    std::queue<IngredientType> available_ingredients; // Queue of ingredients
    std::queue<std::promise<IngredientType>> pending_requests; // Queue of promises (from cooks) waiting for ingredients

public:
    void restock_ingredient(IngredientType ingredient) {
        std::lock_guard<std::mutex> lock(request_mutex);

        // If there are pending requests (by cooks), fulfill the oldest one immediately
        if (!pending_requests.empty()) {
            // Fulfill waiting cook immediately
            auto cook_promise = std::move(pending_requests.front());
            pending_requests.pop();

            cook_promise.set_value(std::move(ingredient));
        } else {
            // Store for later requests
            available_ingredients.push(std::move(ingredient));
        }

        // Mutex is automatically released when lock goes out of scope, allowing other threads to access the queue safely
    }

    // Returns a future that will be fulfilled when the ingredient is available or immediately if already in stock
    // A cook calls this function to request an ingredient. It will block until the ingredient is available.
    std::future<IngredientType> request_ingredient() {
        std::lock_guard<std::mutex> lock(request_mutex); // Lock the mutex to safely check and modify the queues

        if (!available_ingredients.empty()) {
            // Ingredient immediately available
            auto ingredient = std::move(available_ingredients.front());
            available_ingredients.pop();
            
            // Promise future pair 
            std::promise<IngredientType> immediate_promise;
            auto immediate_future = immediate_promise.get_future();

            // Fulfill promise immediately with available ingredient
            immediate_promise.set_value(std::move(ingredient));

            // Return future that is already fulfilled
            return immediate_future;
        } else {
            // Wait for future restock as no ingredient is currently available
            // Create a promise/future pair for this request for ingredients 
            std::promise<IngredientType> request_promise;
            auto request_future = request_promise.get_future();

            pending_requests.push(std::move(request_promise));

            return request_future;
        }
    }
};

void demonstrate_kitchen_coordination() {
    // Create the ingredient request system for tomatoes
    IngredientRequestSystem<TomatoBatch> tomato_system;

    // Prep cook: producer of 5 tomato batches
    std::thread prep_cook([&tomato_system]() {
        for (int batch = 0; batch < 5; ++batch) {
            TomatoBatch fresh_tomatoes{batch, "Roma", 50};

            std::cout << "Prep cook: Processed tomato batch "
                      << batch
                      << std::endl;

            tomato_system.restock_ingredient(std::move(fresh_tomatoes));

            // Tomatoes are restocked every 800ms
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }
    });

    // Multiple line cooks: consumers using futures
    std::vector<std::future<TomatoBatch>> cook_requests;

    // For each cook, request a batch of tomatoes (this will block until the prep cook restocks)
    for (int cook = 0; cook < 5; ++cook) {
        cook_requests.push_back(tomato_system.request_ingredient());
    }

    // Cook threads work with ingredients as they become available
    std::vector<std::thread> line_cooks;

    // Create the ingredient requests 
    for (int cook = 0; cook < 5; ++cook) {

        // Lambda function for each cook thread that waits for their requested tomatoes and simulates cooking
        line_cooks.emplace_back([&cook_requests, cook]() {
            std::cout << "Cook "
                      << cook
                      << " waiting for tomatoes..."
                      << std::endl;

            // Blocks until available
            TomatoBatch tomatoes = cook_requests[cook].get();

            std::cout << "Cook "
                      << cook
                      << " received batch "
                      << tomatoes.batch_id
                      << " with "
                      << tomatoes.quantity
                      << " tomatoes"
                      << std::endl;

            // Simulate cooking time
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            std::cout << "Cook "
                      << cook
                      << " finished cooking"
                      << std::endl;
        });
    }

    // Cleanup
    prep_cook.join();

    for (auto& cook : line_cooks) {
        cook.join();
    }
}

int main() {
    demonstrate_kitchen_coordination();

    return 0;
}