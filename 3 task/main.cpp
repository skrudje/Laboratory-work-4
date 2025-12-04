// main.cpp
#include <iostream>
#include "philosophers.h"

int main() {
    std::cout << "=== Обедающие философы ===\n";
    std::cout << "Решение: ограничение числа сидящих философов до 4 семафором\n";
    std::cout << "Каждый философ делает циклы 'думать и есть'\n\n";

    run_dining_philosophers(5);

    std::cout << "\nВсе философы завершили обед.\n";
    return 0;
}