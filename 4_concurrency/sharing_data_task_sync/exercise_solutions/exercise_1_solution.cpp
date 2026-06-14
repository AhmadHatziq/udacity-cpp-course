/*
Exercise 1: Remote Server Data Fetching Simulation
Lesson 2, Section 2.4

This exercise demonstrates how to use std::promise and std::future to transfer
data between threads by simulating fetching results from a remote server.

To compile and run:
g++ exercise_1_solution.cpp -o running_exercise -pthread && ./running_exercise

Learning objectives:
- Set up a promise/future pair
- Pass the promise to a thread
- Have the thread fulfill the promise (set a value or exception)
- Use future's get() to retrieve the value (blocking until ready)
- Understand thread synchronization through promises/futures
*/

#include <future>
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <random>

// Struct to represent server response data
struct ServerResponse {
    int status_code;
    std::string data;
    std::string server_name;
    
    ServerResponse(int code, const std::string& response_data, const std::string& server)
        : status_code(code), data(response_data), server_name(server) {}
};

// Function that simulates fetching data from a remote server
void fetch_server_data(std::promise<ServerResponse> data_promise, const std::string& request_id) {
    std::cout << "[Server Thread] Starting to process request: " << request_id << std::endl;
    
    // Simulate network delay and server processing time
    std::cout << "[Server Thread] Connecting to remote server..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    
    std::cout << "[Server Thread] Sending request to server..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    
    std::cout << "[Server Thread] Server processing request..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    
    // Simulate generating mock server response
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> status_dist(200, 201); // Success codes
    std::uniform_int_distribution<> server_dist(1, 3);
    
    int status = status_dist(gen);
    int server_num = server_dist(gen);
    
    // Create mock response data
    std::string response_data = "{"
        "\"user_id\": \"12345\", "
        "\"username\": \"johndoe\", "
        "\"email\": \"john@example.com\", "
        "\"last_login\": \"2024-01-15T10:30:00Z\", "
        "\"preferences\": {\"theme\": \"dark\", \"notifications\": true}"
        "}";
    
    std::string server_name = "api-server-" + std::to_string(server_num) + ".example.com";
    
    // Create the response object
    ServerResponse response(status, response_data, server_name);
    
    std::cout << "[Server Thread] Response received from " << server_name << std::endl;
    std::cout << "[Server Thread] Fulfilling promise with server data..." << std::endl;
    
    // Fulfill the promise - this will unblock the future.get() call
    data_promise.set_value(response);
    
    std::cout << "[Server Thread] Promise fulfilled! Data transfer complete." << std::endl;
}

// Function that simulates a server error scenario
void fetch_server_data_with_error(std::promise<ServerResponse> data_promise, const std::string& request_id) {
    std::cout << "[Error Server Thread] Starting to process request: " << request_id << std::endl;
    
    // Simulate network delay
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "[Error Server Thread] Server encountered an error!" << std::endl;
    
    // Set an exception instead of a value
    try {
        throw std::runtime_error("Server timeout: Unable to connect to database");
    } catch (...) {
        data_promise.set_exception(std::current_exception());
    }
    
    std::cout << "[Error Server Thread] Exception set in promise." << std::endl;
}

int main() {
    std::cout << "=== Remote Server Data Fetching Simulation ===" << std::endl;
    std::cout << "Demonstrating std::promise and std::future for thread communication" << std::endl;
    std::cout << std::endl;
    
    // --- SCENARIO 1: Successful data fetch ---
    std::cout << "--- Scenario 1: Successful Data Fetch ---" << std::endl;
    
    // Step 1: Create promise/future pair
    std::promise<ServerResponse> server_promise;
    std::future<ServerResponse> server_future = server_promise.get_future();
    
    std::cout << "[Main Thread] Created promise/future pair" << std::endl;
    std::cout << "[Main Thread] Starting background server request..." << std::endl;
    
    // Step 2: Spawn worker thread with promise
    std::thread server_thread(fetch_server_data, std::move(server_promise), "REQ-001-USER-PROFILE");
    
    std::cout << "[Main Thread] Server thread started. Main thread continuing other work..." << std::endl;
    
    // Simulate main thread doing other work while server request is in progress
    std::cout << "[Main Thread] Doing other work while waiting for server response..." << std::endl;
    for (int i = 1; i <= 3; ++i) {
        std::cout << "[Main Thread] Processing task " << i << "/3..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    
    std::cout << "[Main Thread] Ready to get server response. Calling future.get()..." << std::endl;
    std::cout << "[Main Thread] *** NOTE: get() will BLOCK until promise is fulfilled ***" << std::endl;
    
    // Step 3: Get the result (this blocks until promise is fulfilled)
    try {
        ServerResponse response = server_future.get(); // BLOCKS HERE until promise.set_value() is called
        
        std::cout << std::endl;
        std::cout << "[Main Thread] *** SUCCESS! Received server response ***" << std::endl;
        std::cout << "[Main Thread] Status Code: " << response.status_code << std::endl;
        std::cout << "[Main Thread] Server: " << response.server_name << std::endl;
        std::cout << "[Main Thread] Data: " << response.data << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[Main Thread] Error: " << e.what() << std::endl;
    }
    
    // Step 4: Clean up thread
    server_thread.join();
    
    std::cout << std::endl;
    std::cout << "--- Scenario 2: Server Error Handling ---" << std::endl;
    
    // --- SCENARIO 2: Error handling with exceptions ---
    std::promise<ServerResponse> error_promise;
    std::future<ServerResponse> error_future = error_promise.get_future();
    
    std::cout << "[Main Thread] Starting server request that will fail..." << std::endl;
    
    std::thread error_thread(fetch_server_data_with_error, std::move(error_promise), "REQ-002-TIMEOUT-TEST");
    
    try {
        std::cout << "[Main Thread] Waiting for server response..." << std::endl;
        ServerResponse response = error_future.get(); // This will throw the exception
        std::cout << "[Main Thread] Unexpected success: " << response.data << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "[Main Thread] *** Caught server exception ***" << std::endl;
        std::cout << "[Main Thread] Error details: " << e.what() << std::endl;
    }
    
    error_thread.join();
    
    std::cout << std::endl;
    std::cout << "=== Exercise Complete! ===" << std::endl;
    std::cout << "Key concepts demonstrated:" << std::endl;
    std::cout << "1. Promise/future pair creation and connection" << std::endl;
    std::cout << "2. Passing promise to background thread" << std::endl;
    std::cout << "3. Promise fulfillment with set_value()" << std::endl;
    std::cout << "4. Future blocking behavior with get()" << std::endl;
    std::cout << "5. Exception handling through promises/futures" << std::endl;
    std::cout << "6. Thread synchronization without manual flags" << std::endl;
    
    return 0;
}
