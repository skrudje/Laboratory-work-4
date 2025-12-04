// philosophers.cpp
#include "philosophers.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <condition_variable>

const int NUM_PHILOSOPHERS = 5;

// Простой семафор
class CountingSemaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
public:
    explicit CountingSemaphore(int initial) : count(initial) {}
    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return count > 0; });
        --count;
    }
    void release() {
        std::unique_lock<std::mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }
};

std::vector<std::mutex> forks(NUM_PHILOSOPHERS);
CountingSemaphore table_seats(4); // максимум 4 за столом

void philosopher(int id, int num_cycles) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> think_dist(500, 1500);  
    std::uniform_int_distribution<> eat_dist(100, 300);   

    for (int cycle = 1; cycle <= num_cycles; ++cycle) {  // ← начало с 1
        // Думает
        {
            static std::mutex io_mutex;
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout << "Философ " << id << " думает (раунд " << cycle << ")\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(think_dist(gen)));

        // садится за стол
        table_seats.acquire();

        // Берёт вилки в порядке возрастания индексов
        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;
        if (left > right) std::swap(left, right);

        forks[left].lock();
        forks[right].lock();

        // Ест
        {
            static std::mutex io_mutex;
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout << "Философ " << id << " ест (раунд " << cycle << ")\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(eat_dist(gen)));

        // Кладёт вилки
        forks[left].unlock();
        forks[right].unlock();
        table_seats.release();
    }
}

void run_dining_philosophers(int num_cycles) {
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        threads.emplace_back(philosopher, i, num_cycles);
    }
    for (auto& t : threads) t.join();
}