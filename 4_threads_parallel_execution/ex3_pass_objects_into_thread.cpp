/* To run this program, use the following command:
g++ exercise_3_solution.cpp -o exercise_3
./exercise_3

Expected output: 
>>> Starting main thread
>>> Original thread_id value: 42
>>> Value of thread_id within the pass by value function: 126. // Local copy is 42 x 3 
>>> Original thread_id value after pass by value: 42 // Original value is unchanged
>>> Value of thread_id within the pass by reference function: 336 // There is lambda function and std::ref
>>> Original thread_id value after pass by reference: 336
>>> Main thread ending
*/

#include <iostream>
#include <thread>

void pass_by_value(int thread_id)
{
    thread_id = thread_id * 3;
    std::cout << ">>> Value of thread_id within the pass by value function: " << thread_id << std::endl;

}

int main()
{
    std::cout << ">>> Starting main thread" << std::endl;

    int thread_id = 42;
    std::cout << ">>> Original thread_id value: " << thread_id << std::endl;

    std::thread t1(pass_by_value, thread_id);
    t1.join();
    std::cout << ">>> Original thread_id value after pass by value: " << thread_id << std::endl;

    std::thread t2([](int& thread_id) {
        thread_id = thread_id * 8;
        std::cout << ">>> Value of thread_id within the pass by reference function: " << thread_id << std::endl;
    }, std::ref(thread_id));
    t2.join();
    std::cout << ">>> Original thread_id value after pass by reference: " << thread_id << std::endl;

    std::cout << ">>> Main thread ending" << std::endl;
    return 0;
}
