#include "ThreadSafeCounter.h"
#include <iostream>

ThreadSafeCounter::ThreadSafeCounter() : shared_counter(0) {
    if (pthread_spin_init(&lock, 0) != 0) {  
        std::cerr << "自旋锁初始化失败" << std::endl;
    }
}

ThreadSafeCounter::~ThreadSafeCounter() {
    int ret = pthread_spin_destroy(&lock);
    if (ret != 0) {
        std::cerr << "自旋锁销毁失败" << std::endl;
    }
}

int ThreadSafeCounter::increment() {
    pthread_spin_lock(&lock);
    shared_counter++;
    pthread_spin_unlock(&lock);
    return shared_counter;
}

int ThreadSafeCounter::get() const {
    pthread_spin_lock(&lock);
    int temp = shared_counter;
    pthread_spin_unlock(&lock);
    return temp;
}
