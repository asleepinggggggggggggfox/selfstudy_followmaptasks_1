#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // 用于 usleep

int shared_counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* problematic_increment(void* arg) {
    for (int i = 0; i < 1000; i++) { 

        pthread_mutex_lock(&lock);

        int temp = shared_counter;

    
        usleep(1); // 休眠约1微秒

        temp = temp + 1;


        usleep(1);


        shared_counter = temp;
        pthread_mutex_unlock(&lock);

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