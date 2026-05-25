#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <atomic>

template <typename T>
class BlockQueue {
public:
    explicit BlockQueue(size_t cap = 10) : capacity_(cap), stop_(false) {}

    void push(T item) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_full_.wait(lock, [this] { return stop_ || queue_.size() < capacity_; });
        if (stop_) return;
        queue_.push(std::move(item));
        lock.unlock();
        not_empty_.notify_one();
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_empty_.wait(lock, [this] { return stop_ || !queue_.empty(); });
        if (stop_ && queue_.empty()) return false;
        item = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        not_full_.notify_one();
        return true;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    size_t capacity_;
    bool stop_;
};

inline void demo() {
    BlockQueue<int> bq(5);
    std::atomic<int> counter(0);

    std::thread consumer([&] {
        int val;
        while (bq.pop(val))
            std::cout << std::this_thread::get_id() << " consume: " << val << std::endl;
    });

    std::thread producer([&] {
        for (int i = 0; i < 20; i++) {
            bq.push(counter++);
            std::cout << std::this_thread::get_id() << " produce: " << i << std::endl;
        }
        bq.stop();
    });

    producer.join();
    consumer.join();
}
