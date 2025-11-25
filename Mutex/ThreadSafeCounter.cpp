#include <pthread.h>
#include "ThreadSafeCounter.h"

ThreadSafeCounter::ThreadSafeCounter() : shared_counter(0) {
        if (pthread_mutex_init(&lock, nullptr) != 0) {
            // 处理初始化失败，例如抛出异常或记录错误
        }
    }

ThreadSafeCounter::~ThreadSafeCounter() {
        int ret = pthread_mutex_destroy(&lock);
        if (ret != 0) {
        // 处理销毁失败，例如记录错误（不应在析构中抛出异常）
        // EBUSY 表示锁已被锁定或正在被使用，可以尝试解锁后再销毁，但需谨慎评估逻辑
        }
    }
        int ThreadSafeCounter::increment(){
            pthread_mutex_lock(&lock);
            shared_counter++;
            pthread_mutex_unlock(&lock);
            return shared_counter;
        }

        int ThreadSafeCounter::get() const{
            pthread_mutex_lock(&lock);
            int temp = shared_counter;
            pthread_mutex_unlock(&lock);
            return temp;
        }



