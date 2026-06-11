 /* To run this program, use the following command:
 g++ exercise_2_solution.cpp -o exercise_2
 ./exercise_2

 >>> Starting main thread
 Hello from thread 1
 Hello from thread 2
 Hello from thread 3
 >>> Main thread pausing for 5 seconds to show detached thread working...
 Detached thread is working... 1
 Detached thread is working... 2
 Detached thread is working... 3
 Detached thread is working... 4
 Detached thread is working... 5
 Detached thread is working... 6
 Detached thread is working... 7
 Detached thread is working... 8
 Detached thread is working... 9
 Detached thread is working... 10
 >>> Main thread ending
*/

#include <iostream>
#include <thread>
#include <chrono>

void thread_function(int thread_id)
{
    std::cout << "Hello from thread " << thread_id << std::endl;
}

void detached_thread_function()
{
    int counter = 1;
    while (true) {
        std::cout << "Detached thread is working... " << counter++ << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // optional. prevents the system from printing out so many messages we can't see the entire output.
    }
}

int main()
{
    std::cout << ">>> Starting main thread" << std::endl;
    
    std::thread t1(thread_function, 1);
    std::thread t2(thread_function, 2);
    std::thread t3(thread_function, 3);
    
    std::thread t4(detached_thread_function);
    t4.detach();
    
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << ">>> Main thread pausing for 5 seconds to show detached thread working..." << std::endl;
     // Any of these will pause the program so we can see the detached thread working:
    std::this_thread::sleep_for(std::chrono::seconds(5)); // pauses the main thread for 5 seconds
     std::cin.get();   // Wait for user to press Enter

    
    std::cout << ">>> Main thread ending" << std::endl;
    return 0;
}
