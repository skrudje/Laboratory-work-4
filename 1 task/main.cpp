// main.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include "primitives.h"

int main() {
    std::cout << "Потоков: " << NUM_THREADS << ", Итераций: " << TOTAL_OPERATIONS << "\n\n";

    struct { const char* name; long long time; } results[] = {
        {"Mutex",     test_mutex()},
        {"Semaphore", test_semaphore()},
        {"Barrier",   test_barrier()},
        {"SpinLock",  test_spinlock()},
        {"SpinWait",  test_spinwait()},
        {"Monitor",   test_monitor()}
    };

    std::cout << "Результаты (время в микросекундах) \n";
    std::cout << std::setw(12) << " Примитив" << "    |      " << std::setw(10) << "Время\n";
    std::cout << "--------------------------------\n";
    for (auto& r : results) {
        std::cout << std::setw(12) << r.name << " | " << std::setw(10) << r.time << "\n";
    }

    return 0;
}