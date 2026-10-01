#include <algorithm>           // std::all_of
#include <atomic>              // std::atomic
#include <chrono>
#include <condition_variable>  // std::condition_variable
#include <exception>           // std::exception_ptr, current_exception, rethrow_exception
#include <iostream>
#include <mutex>               // std::mutex, unique_lock, lock_guard
#include <optional>            // std::optional (requires C++17)
#include <string>
#include <thread>
#include <vector>

// Run with: 
// g++ -std=c++17 -Wall -Wextra -pthread recipe_example.cpp -o demo && ./demo

template<typename RecipeResult>
class CollaborativeRecipeManager {
private:
    std::mutex recipe_coordination_mutex;
    // std::condition_variable chef_assignment_ready;
    std::condition_variable recipe_completion_ready;
    
    std::optional<std::thread::id> lead_chef;
    std::optional<RecipeResult> shared_recipe_result;
    std::exception_ptr recipe_preparation_error;
    bool recipe_development_complete = false;
    std::atomic<int> participating_chefs{0};
    
public:
    template<typename RecipeFunction>
    RecipeResult develop_collaborative_recipe(RecipeFunction&& recipe_development_work) {
        participating_chefs.fetch_add(1);
        std::unique_lock<std::mutex> lock(recipe_coordination_mutex);
        
        // Wait for chef assignment or recipe completion
        // Removed as initially, lead_chef is empty and recipe_development_complete is false. 
        // Every thread therefore waits, but the code that assigns the lead chef is after that wait. Nobody can reach it.
        /*
        chef_assignment_ready.wait(lock, [this] {
            return lead_chef.has_value() || recipe_development_complete;
        });
        */
        
        if (!recipe_development_complete && 
            (!lead_chef.has_value() || lead_chef.value() == std::this_thread::get_id())) {
            
            // This chef (which first acquires the mutex) becomes the lead developer
            lead_chef = std::this_thread::get_id();
            
            std::cout << "Chef " << std::this_thread::get_id() 
                      << " assigned lead recipe development" << std::endl;
            
            lock.unlock();  // Release coordination during expensive development
            
            try {
                // Lead chef does work via the function passed as the argument 
                RecipeResult developed_recipe = recipe_development_work();
                
                {
                    std::lock_guard<std::mutex> completion_lock(recipe_coordination_mutex);
                    shared_recipe_result = developed_recipe;
                    recipe_development_complete = true;
                }
                
                recipe_completion_ready.notify_all();  // Share result with all participating chefs
                participating_chefs.fetch_sub(1);
                return developed_recipe;
                
            } catch (...) {
                {
                    std::lock_guard<std::mutex> error_lock(recipe_coordination_mutex);
                    recipe_preparation_error = std::current_exception();
                    recipe_development_complete = true;
                }
                
                recipe_completion_ready.notify_all();
                participating_chefs.fetch_sub(1);
                std::rethrow_exception(recipe_preparation_error);
            }
            
        } else {
            // Block for non-lead chefs: 
            // Wait for lead chef to complete development
            std::cout << "Chef " << std::this_thread::get_id() 
                      << " waiting for collaborative recipe completion" << std::endl;
            
            // Condition variable: Will wait until recipe_development_complete is true (set by lead chef)
            recipe_completion_ready.wait(lock, [this] { return recipe_development_complete; });
            
            participating_chefs.fetch_sub(1);
            
            if (recipe_preparation_error) {
                std::rethrow_exception(recipe_preparation_error);
            }
            
            return shared_recipe_result.value();  // Use recipe developed by lead chef
        }
    }
    
    int get_active_participants() const {
        return participating_chefs.load();
    }
    
    void reset_for_next_recipe_development() {
        std::lock_guard<std::mutex> lock(recipe_coordination_mutex);
        lead_chef.reset();
        shared_recipe_result.reset();
        recipe_preparation_error = nullptr;
        recipe_development_complete = false;
        participating_chefs.store(0);
    }
};

struct SignatureRecipe {
    std::string recipe_name;
    std::vector<std::string> ingredients;
    std::vector<std::string> preparation_steps;
    int complexity_level;
    std::chrono::milliseconds development_time;
    std::thread::id developed_by_chef;
};

void demonstrate_collaborative_recipe_development() {
    CollaborativeRecipeManager<SignatureRecipe> recipe_coordinator;
    
    const int number_of_participating_chefs = 6;
    std::vector<std::thread> chef_threads;
    std::vector<SignatureRecipe> chef_recipe_results(number_of_participating_chefs);
    std::atomic<int> chefs_completed{0};
    
    std::cout << "Starting collaborative signature recipe development with " 
              << number_of_participating_chefs << " chefs" << std::endl;
    
    // Launch participating chefs
    for (int chef_id = 0; chef_id < number_of_participating_chefs; ++chef_id) {
        chef_threads.emplace_back([&, chef_id]() {
            auto development_start_time = std::chrono::steady_clock::now();
            
            try {
                chef_recipe_results[chef_id] = recipe_coordinator.develop_collaborative_recipe([chef_id]() {

                    // Lambda function for developing recipe 
                    std::cout << "Chef " << std::this_thread::get_id() 
                              << " performing expensive signature recipe development..." << std::endl;
                    
                    // Simulate complex recipe development process
                    std::this_thread::sleep_for(std::chrono::seconds(4));
                    
                    SignatureRecipe signature_recipe;
                    signature_recipe.recipe_name = "Collaborative Signature Dish XXX";
                    signature_recipe.ingredients = {
                        "Premium beef tenderloin", "Wild mushrooms", "Truffle oil", 
                        "Heirloom tomatoes", "Fresh herbs", "Aged balsamic"
                    };
                    signature_recipe.preparation_steps = {
                        "Sear beef to perfection", "Sauté mushrooms with truffle oil",
                        "Create reduction sauce", "Plate with artistic presentation"
                    };
                    signature_recipe.complexity_level = 8;
                    signature_recipe.development_time = std::chrono::seconds(4);
                    signature_recipe.developed_by_chef = std::this_thread::get_id();
                    
                    return signature_recipe;
                });
                
                auto development_end_time = std::chrono::steady_clock::now();
                auto total_wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    development_end_time - development_start_time);
                
                std::cout << "Chef " << chef_id << " received signature recipe after " 
                          << total_wait_time.count() << "ms wait" << std::endl;
                
                chefs_completed.fetch_add(1);
                
            } catch (const std::exception& exception) {
                std::cout << "Chef " << chef_id << " encountered error: " 
                          << exception.what() << std::endl;
                chefs_completed.fetch_add(1);
            }
        });
    }
    
    // Monitor development progress
    std::thread development_monitor([&recipe_coordinator, &chefs_completed, number_of_participating_chefs]() {
        while (chefs_completed.load() < number_of_participating_chefs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            std::cout << "Development Progress - Active participants: " 
                      << recipe_coordinator.get_active_participants() 
                      << ", Completed: " << chefs_completed.load() 
                      << "/" << number_of_participating_chefs << std::endl;
        }
    });
    
    // Wait for all chefs + development_monitor thread to complete
    for (auto& chef_thread : chef_threads) {
        chef_thread.join();
    }
    development_monitor.join();
    
    // Verify all chefs received identical recipes
    bool all_recipes_identical = std::all_of(
        chef_recipe_results.begin() + 1, 
        chef_recipe_results.end(),
        [&chef_recipe_results](const SignatureRecipe& recipe) {
            return recipe.recipe_name == chef_recipe_results[0].recipe_name &&
                   recipe.complexity_level == chef_recipe_results[0].complexity_level &&
                   recipe.developed_by_chef == chef_recipe_results[0].developed_by_chef;
        }
    );
    
    std::cout << "\n=== Recipe Development Results ===" << std::endl;
    std::cout << "All chefs received identical recipe: " << std::boolalpha 
              << all_recipes_identical << std::endl;
    std::cout << "Recipe name: " << chef_recipe_results[0].recipe_name << std::endl;
    std::cout << "Developed by chef: " << chef_recipe_results[0].developed_by_chef << std::endl;
    std::cout << "Complexity level: " << chef_recipe_results[0].complexity_level << std::endl;
    std::cout << "Development time: " << chef_recipe_results[0].development_time.count() 
              << "ms" << std::endl;
    std::cout << "Ingredients: " << chef_recipe_results[0].ingredients.size() << std::endl;
    std::cout << "Preparation steps: " << chef_recipe_results[0].preparation_steps.size() << std::endl;
    
    // Demonstrate system efficiency
    std::cout << "Coordination efficiency: One development process served " 
              << number_of_participating_chefs << " requesting chefs" << std::endl;
    
    std::cout << "Recipe ingredients:" << std::endl;
    for (const auto& ingredient : chef_recipe_results[0].ingredients) {
        std::cout << "  - " << ingredient << std::endl;
    }
}

int main() {
    demonstrate_collaborative_recipe_development();
    return 0;
}