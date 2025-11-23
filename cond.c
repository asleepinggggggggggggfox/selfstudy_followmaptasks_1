#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition_flag = 0; // 条件谓词

void* waiting_thread(void* arg) {
    pthread_mutex_lock(&mutex); // 1. 加锁
    printf("等待线程：开始检查条件...\n");
    
    while (condition_flag == 0) { // 2. 使用 WHILE 循环检查条件
        printf("条件不满足，进入等待...\n");
        pthread_cond_wait(&cond, &mutex); // 3. 原子性地解锁并等待
        printf("等待线程：被唤醒，重新检查条件。\n");
    }
    
    printf("等待线程：条件满足！继续执行。\n");
    pthread_mutex_unlock(&mutex); // 4. 解锁
    return NULL;
}

void* notifying_thread(void* arg) {
    sleep(2); // 模拟一些准备工作
    pthread_mutex_lock(&mutex); // 1. 加锁
    printf("通知线程：设置条件标志。\n");
    condition_flag = 1; // 2. 修改条件
    pthread_cond_signal(&cond); // 3. 发送信号（通常在持有锁时调用）
    printf("通知线程：已发送通知信号。\n");
    pthread_mutex_unlock(&mutex); // 4. 解锁
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, waiting_thread, NULL);
    pthread_create(&t2, NULL, notifying_thread, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}