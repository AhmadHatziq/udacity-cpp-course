#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <chrono>

/* 
    g++ -std=c++17 demo_consumer_producer_queue.cpp -pthread -o demo && ./demo

*/

template <typename T>
class ProducerConsumerQueue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;

public:
    // Producer operation: add item to queue
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            queue_.push(item);
        }

        // Wake one waiting consumer
        condition_.notify_one();
    }

    // Consumer operation: remove item from queue
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        // Wait until queue has data available
        condition_.wait(lock, [this] {
            return !queue_.empty();
        });

        // At this point, the mutex is locked
        // and the queue is guaranteed to contain data.
        T result = queue_.front();
        queue_.pop();

        return result;
    }
};

int main() {
    ProducerConsumerQueue<int> queue;

    // Producer thread
    std::thread producer([&queue]() {
        for (int i = 1; i <= 5; ++i) {
            std::cout << "Producer creating: " << i << '\n';

            queue.push(i);

            std::this_thread::sleep_for(
                std::chrono::milliseconds(500)
            );
        }
    });

    // Consumer thread
    std::thread consumer([&queue]() {
        for (int i = 1; i <= 5; ++i) {
            int value = queue.pop();

            std::cout << "Consumer received: " << value << '\n';
        }
    });

    producer.join();
    consumer.join();

    return 0;
}