// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "students.h"

int main() {
    size_t n;
    int k_threshold;
    int num_threads;

    std::cout << "--- Студенты на отчисление ---\n";

    std::cout << "Введите общее количество студентов: ";
    std::cin >> n;

    std::cout << "Введите пороговый курс K (отчисляются, если курс > K): ";
    std::cin >> k_threshold;

    std::cout << "Введите количество потоков: ";
    std::cin >> num_threads;

    std::cout << "\nУсловие: долги > " << DEBTS_THRESHOLD << " и курс > " << k_threshold << "\n";
    std::cout << "Общее количество студентов: " << n << "\n";
    std::cout << "Потоков: " << num_threads << "\n\n";

    auto students = generate_students(n);

    // Однопоточная обработка
    auto start = std::chrono::high_resolution_clock::now();
    auto result_single = single_threaded(students, k_threshold);
    auto end = std::chrono::high_resolution_clock::now();
    auto time_single = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Многопоточная обработка
    start = std::chrono::high_resolution_clock::now();
    auto result_multi = multi_threaded(students, k_threshold);
    end = std::chrono::high_resolution_clock::now();
    auto time_multi = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Однопоточно: " << time_single << " мкс\n";
    std::cout << "Многопоточно: " << time_multi << " мкс\n";
    std::cout << "Найдено: " << result_multi.size() << " студентов\n";
    std::cout << "Найдено: " << result_single.size() << " студентов\n";

    if (!result_multi.empty()) {
        std::cout << "\nПримеры ФИО:\n";
        print_sample(result_multi, 10);
    }

    return 0;
}