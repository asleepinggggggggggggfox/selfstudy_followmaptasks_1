#ifndef THREADSAFECOUNTER_H
#define THREADSAFECOUNTER_H

#include <pthread.h>
#include <atomic>

class ThreadSafeCounter{
    private:
        std::atomic<int> shared_counter;
    public:
    ThreadSafeCounter();   // 构造函数声明
    ~ThreadSafeCounter();  // 析构函数声明
    
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

#endif