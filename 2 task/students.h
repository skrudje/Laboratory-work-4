// students.h
#ifndef STUDENTS_H
#define STUDENTS_H

#include <vector>
#include <string>

struct Student {
    std::string fio;
    int course;
    int debts;
};


// долги > 3
constexpr int DEBTS_THRESHOLD = 3;

// Функции
std::vector<Student> generate_students(size_t n);
bool should_be_expelled(const Student& s, int k_threshold); // теперь передаём K
std::vector<std::string> single_threaded(const std::vector<Student>& students, int k_threshold);
std::vector<std::string> multi_threaded(const std::vector<Student>& students, int k_threshold);
void print_sample(const std::vector<std::string>& list, size_t max = 10);

#endif // STUDENTS_H