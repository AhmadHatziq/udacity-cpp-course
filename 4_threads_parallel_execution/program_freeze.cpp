/*
To run this program, use the following command:
g++ eternal_pause.cpp -o eternal_pause
./eternal_pause

This program demonstrates unsafe multithreading that creates an eternal pause.
Thread 1 acquires a lock and never releases it, causing other threads to wait forever.

Expected output:
Thread 1: Got lock, sleeping forever...
Thread 2: Waiting for lock...
Thread 3: Waiting for lock...
[Program hangs here - threads 2-3 wait eternally]
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex m;

void thread_func(int id) {
    if (id == 1) {
        // Thread 1: Acquire lock and never release it (UNSAFE!)
        m.lock();
        std::cout << "Thread " << id << ": Got lock, sleeping forever..." << std::endl;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Never calls m.unlock() - this is the bug!
        }
    } else {
        // Other threads: Wait forever for the lock
        std::cout << "Thread " << id << ": Waiting for lock..." << std::endl;
        std::lock_guard<std::mutex> lock(m);
        std::cout << "Thread " << id << ": Got lock!" << std::endl; // Never prints
    }
}

int main() {
    std::thread t1(thread_func, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Let t1 go first
    
    std::thread t2(thread_func, 2);
    std::thread t3(thread_func, 3);
    
    std::cout << "Press Ctrl+C to exit..." << std::endl;
    
    t1.join();
    t2.join();
    t3.join();
    
    return 0; // Never reached
}
