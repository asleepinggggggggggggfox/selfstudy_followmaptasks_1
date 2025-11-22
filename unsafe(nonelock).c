#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // 用于 usleep

int shared_counter = 0;

// 一个非原子、非线程安全的复杂操作：模拟"读取-修改-写入"过程
void* problematic_increment(void* arg) {
    for (int i = 0; i < 1000; i++) { // 循环次数无需太多
        // 1. 读取全局变量的值到一个局部变量
        int temp = shared_counter;

        // 2. 【关键】模拟一个耗时的操作，极大增加线程切换的概率
        // 这给了另一个线程在当前线程"读取"和"写入"之间介入的机会
        usleep(1); // 休眠约1微秒

        // 3. 修改局部变量
        temp = temp + 1;

        // 4. 再次模拟耗时操作，增加不确定性
        usleep(1);

        // 5. 将局部变量的值写回全局变量
        shared_counter = temp;
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

    printf("实际运行结果: %d\n", shared_counter);
    printf("数据丢失了 %d 次增加操作。\n", 3000 - shared_counter);

    return 0;
}