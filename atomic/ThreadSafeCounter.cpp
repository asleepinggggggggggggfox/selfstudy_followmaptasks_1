#include <pthread.h>
#include <atomic>
#include "ThreadSafeCounter.h"

ThreadSafeCounter::ThreadSafeCounter() : shared_counter(0) { // 使用成员初始化列表

}

// 析构函数定义
ThreadSafeCounter::~ThreadSafeCounter() {

}

              
int ThreadSafeCounter::increment(){
    return shared_counter.fetch_add(1);
}
int ThreadSafeCounter::get() const{
    int a = shared_counter.load();
    return a;
}
