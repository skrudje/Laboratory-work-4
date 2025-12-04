// primitives.cpp
#include "primitives.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>
#include <chrono>
#include <algorithm>

const int NUM_THREADS = 6;
const int TOTAL_OPERATIONS = 1000000;

// генератор
static std::random_device rd;
static thread_local std::mt19937 gen(rd());

class SimpleMutex {
    std::mutex mtx;
public:
    void lock() { mtx.lock(); }
    void unlock() { mtx.unlock(); }
};

class SimpleSemaphore {
    std::mutex mtx;
    std::condition_variable cv;
    int count;
public:
    explicit SimpleSemaphore(int initial = 1) : count(initial) {}
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count <= 0) cv.wait(lock);
        --count;
    }
    void signal() {
        std::unique_lock<std::mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }
};

class SimpleBarrier {
    std::mutex mtx;
    std::condition_variable cv;
    int count, total, epoch = 0;
public:
    explicit SimpleBarrier(int n) : total(n), count(n) {}
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        int e = epoch;
        if (--count == 0) {
            count = total;
            ++epoch;
            cv.notify_all();
        } else {
            cv.wait(lock, [this, e] { return epoch != e; });
        }
    }
};

class SimpleSpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {}
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

class SimpleSpinWaitLock {
    std::atomic<bool> locked{false};
public:
    void lock() {
        while (locked.exchange(true, std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }
    void unlock() {
        locked.store(false, std::memory_order_release);
    }
};

class SimpleMonitor {
    std::mutex mtx;
public:
    template<typename Func>
    void execute(Func&& f) {
        std::lock_guard<std::mutex> lock(mtx);
        f();
    }
};

// распределение диапазона
std::pair<int, int> get_range(int thread_id, int total_threads, int total_ops) {
    int chunk = total_ops / total_threads;
    int remainder = total_ops % total_threads;
    int start = thread_id * chunk + std::min(thread_id, remainder);
    int end = start + chunk + (thread_id < remainder ? 1 : 0);
    return {start, end};
}

// тесты с распределённой нагрузкой
long long test_mutex() {
    SimpleMutex m;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            auto [start, end] = get_range(i, NUM_THREADS, TOTAL_OPERATIONS);
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            for (int j = start; j < end; ++j) {
                m.lock();
                char c = static_cast<char>(dis(gen));
                dummy = c;
                m.unlock();
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}

long long test_semaphore() {
    SimpleSemaphore s;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            auto [start, end] = get_range(i, NUM_THREADS, TOTAL_OPERATIONS);
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            for (int j = start; j < end; ++j) {
                s.wait();
                char c = static_cast<char>(dis(gen));
                dummy = c;
                s.signal();
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}

long long test_barrier() {

    SimpleBarrier barrier(NUM_THREADS);
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&]() {
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            // Каждый поток проходит TOTAL_OPERATIONS фаз
            for (int j = 0; j < TOTAL_OPERATIONS; ++j) {
                barrier.wait(); // все ждут друг друга
                char c = static_cast<char>(dis(gen));
                dummy = c;
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}

long long test_spinlock() {
    SimpleSpinLock sl;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            auto [start, end] = get_range(i, NUM_THREADS, TOTAL_OPERATIONS);
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            for (int j = start; j < end; ++j) {
                sl.lock();
                char c = static_cast<char>(dis(gen));
                dummy = c;
                sl.unlock();
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}

long long test_spinwait() {
    SimpleSpinWaitLock sw;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            auto [start, end] = get_range(i, NUM_THREADS, TOTAL_OPERATIONS);
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            for (int j = start; j < end; ++j) {
                sw.lock();
                char c = static_cast<char>(dis(gen));
                dummy = c;
                sw.unlock();
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}

long long test_monitor() {
    SimpleMonitor mon;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            auto [start, end] = get_range(i, NUM_THREADS, TOTAL_OPERATIONS);
            std::uniform_int_distribution<> dis(32, 126);
            volatile char dummy = 0;
            for (int j = start; j < end; ++j) {
                mon.execute([&]() {
                    char c = static_cast<char>(dis(gen));
                    dummy = c;
                });
            }
            (void)dummy;
        });
    }

    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start
    ).count();
}