#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 
#include <vector>
class ThreadSafeCounter{
    private:
        int shared_counter;
        mutable pthread_mutex_t lock;
    public:
    ThreadSafeCounter() : shared_counter(0) {
        if (pthread_mutex_init(&lock, nullptr) != 0) {
            // 处理初始化失败，例如抛出异常或记录错误
        }
    }

    ~ThreadSafeCounter() {
        int ret = pthread_mutex_destroy(&lock);
        if (ret != 0) {
        // 处理销毁失败，例如记录错误（不应在析构中抛出异常）
        // EBUSY 表示锁已被锁定或正在被使用，可以尝试解锁后再销毁，但需谨慎评估逻辑
        }
    }
        //禁止拷贝和复制操作以实现线程安全
        // ThreadSafeCounter counter2 = counter1; // 错误：尝试使用被删除的拷贝构造函数
        // ThreadSafeCounter counter3(counter1);   // 错误：同上
        ThreadSafeCounter& operator=(const ThreadSafeCounter&) = delete;
        // counter1 = another_counter;             // 错误：尝试使用被删除的拷贝赋值运算符
        ThreadSafeCounter(const ThreadSafeCounter&) = delete;

        int increment(){
            pthread_mutex_lock(&lock);
            shared_counter++;
            pthread_mutex_unlock(&lock);
            return shared_counter;
        }

        int get() const{
            pthread_mutex_lock(&lock);
            int temp = shared_counter;
            pthread_mutex_unlock(&lock);
            return temp;
        }


};


