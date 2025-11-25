#ifndef THREADSAFECOUNTER_H
#define THREADSAFECOUNTER_H

#include <pthread.h>

class ThreadSafeCounter {
private:
    int shared_counter;
    mutable pthread_spinlock_t lock; // mutable 允许在 const 成员函数中修改锁

public:
    // 构造函数
    ThreadSafeCounter();
    // 析构函数
    ~ThreadSafeCounter();

    // 禁止拷贝构造和拷贝赋值操作以确保线程安全[2](@ref)
    ThreadSafeCounter(const ThreadSafeCounter&) = delete;
    ThreadSafeCounter& operator=(const ThreadSafeCounter&) = delete;

    // 递增计数器并返回新值
    int increment();
    // 获取当前计数器值
    int get() const;
};

#endif // THREADSAFECOUNTER_H