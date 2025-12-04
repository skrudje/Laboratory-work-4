// students.cpp
#include "students.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <thread>
#include <vector>

std::vector<Student> generate_students(size_t n) {
    std::vector<Student> students;
    students.reserve(n);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> course_dist(1, 6);
    std::uniform_int_distribution<> debts_dist(0, 5);


    for (size_t i = 1; i <= n; ++i) {
        students.push_back({
            "Student_" + std::to_string(i),
            course_dist(gen),
            debts_dist(gen)
        });
    }
    return students;
}   

bool should_be_expelled(const Student& s, int k_threshold) {
    return s.debts > DEBTS_THRESHOLD && s.course > k_threshold;
}

std::vector<std::string> single_threaded(const std::vector<Student>& students, int k_threshold) {
    std::vector<std::string> result;
    for (const auto& s : students) {
        if (should_be_expelled(s, k_threshold)) {
            result.push_back(s.fio);
        }
    }
    return result;
}

std::vector<std::string> multi_threaded(const std::vector<Student>& students, int k_threshold) {
    size_t n = students.size();
    if (n == 0) return {};

    // количество потоков
    const int NUM_THREADS = 4;

    size_t chunk = n / NUM_THREADS;
    size_t remainder = n % NUM_THREADS;

    std::vector<std::thread> threads;
    std::vector<std::vector<std::string>> thread_results(NUM_THREADS);

    for (int t = 0; t < NUM_THREADS; ++t) {
        size_t start = t * chunk + std::min(static_cast<size_t>(t), remainder);
        size_t end = start + chunk + (t < remainder ? 1 : 0);

        threads.emplace_back([&, t, start, end, k_threshold]() {
            std::vector<std::string> local;
            for (size_t i = start; i < end; ++i) {
                if (should_be_expelled(students[i], k_threshold)) {
                    local.push_back(students[i].fio);
                }
            }
            thread_results[t] = std::move(local);
        });
    }

    for (auto& t : threads) t.join();

    std::vector<std::string> result;
    for (auto& part : thread_results) {
        result.insert(result.end(), part.begin(), part.end());
    }
    return result;
}

void print_sample(const std::vector<std::string>& list, size_t max) {
    size_t to_print = std::min(max, list.size());
    for (size_t i = 0; i < to_print; ++i) {
        std::cout << list[i] << "\n";
    }
    if (list.size() > max) {
        std::cout << "... и ещё " << (list.size() - max) << " студентов\n";
    }
}