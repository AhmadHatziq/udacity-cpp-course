/*
To run this program, use the following command:
g++ exercise_1_solution.cpp -o exercise_1
./exercise_1

--- The output is nondeterministic. Examples include: ---
>>> Starting main thread
Hello from a funtion call. Thread number: 1
Hello from a funtion call. Thread number: 2
Hello from a funtion call. Thread number: 3
>>> Main thread ending
terminate called without an active exception
Hello from a lambda thread!
Aborted (core dumped)

--- Another example is here. Note how threads 1 and 2 are mixed together. ---
>>> Starting main thread
Hello from thread Hello from thread 21

>>> Main thread ending
terminate called without an active exception
Hello from thread 3
Aborted (core dumped)
*/

#include <iostream>
#include <thread>

void thread_function(int thread_id)
{
    std::cout << "Hello from a funtion call. Thread number: " << thread_id << std::endl;
}

int main()
{
    std::cout << ">>> Starting main thread" << std::endl;
    
    std::thread t1(thread_function, 1);
    std::thread t2(thread_function, 2);
    std::thread t3(thread_function, 3);

    std::thread t4([]() {
        std::cout << "Hello from a lambda thread!" << std::endl;
    });
    
    std::cout << ">>> Main thread ending" << std::endl;
    return 0;
}
