#ifndef THREADSAFECOUNTER_H
#define THREADSAFECOUNTER_H

#include <pthread.h>

/**
 * @brief 线程安全的计数器类
 * 
 * 该类使用互斥锁(pthread_mutex_t)确保多线程环境下对计数器的操作是安全的。
 * 禁止拷贝构造和赋值操作以避免潜在的线程安全问题。
 */
class ThreadSafeCounter {
private:
    int shared_counter;               ///< 共享计数器
    mutable pthread_mutex_t lock;     ///< 互斥锁，mutable允许const成员函数修改

public:
    /**
     * @brief 默认构造函数，初始化计数器和互斥锁
     */
    ThreadSafeCounter();
    
    /**
     * @brief 析构函数，销毁互斥锁
     */
    ~ThreadSafeCounter();
    
    // 禁止拷贝构造和赋值操作
    ThreadSafeCounter(const ThreadSafeCounter&) = delete;
    ThreadSafeCounter& operator=(const ThreadSafeCounter&) = delete;
    
    /**
     * @brief 原子性地递增计数器
     * @return 递增后的计数器值
     */
    int increment();
    
    /**
     * @brief 获取当前计数器值
     * @return 当前的计数器值
     */
    int get() const;
};

#endif // THREADSAFECOUNTER_H