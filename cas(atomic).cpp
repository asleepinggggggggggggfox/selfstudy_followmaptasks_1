#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // 用于 usleep
#include <atomic>

std::atomic<int> shared_counter{0};
              
void* problematic_increment(void* arg) {
    for (int i = 0; i < 1000; i++) {
        int expected = shared_counter.load(std::memory_order_relaxed); // 1. 读取当前值
        // 2. 循环重试，直到成功为止
        while (!shared_counter.compare_exchange_strong(expected, expected + 1,
                                                       std::memory_order_release,
                                                       std::memory_order_relaxed)) {
            // 循环体在CAS失败时通常会为空
            // 因为CAS失败时，`expected`已经被更新为`shared_counter`的最新值
            // 循环会立即用这个新值再次尝试
        }
        usleep(1);
    }
    return NULL;
}
int main() {
    pthread_t t1, t2, t3; // 使用三个线程，竞争更激烈

    printf("启动3个线程，每个线程执行1000次增加操作。\n");
    printf("理论预期结果: 3000\n");

    pthread_create(&t1, NULL, problematic_increment, NULL);
    pthread_create(&t2, NULL, problematic_increment, NULL);
    pthread_create(&t3, NULL, problematic_increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("实际运行结果: %d\n", shared_counter.load());
    printf("数据丢失了 %d 次增加操作。\n", 3000 - shared_counter.load());

    return 0;
}