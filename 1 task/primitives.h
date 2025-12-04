// primitives.h
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

extern const int NUM_THREADS;
extern const int TOTAL_OPERATIONS;

long long test_mutex();
long long test_semaphore();
long long test_barrier();
long long test_spinlock();
long long test_spinwait();
long long test_monitor();

#endif // PRIMITIVES_H