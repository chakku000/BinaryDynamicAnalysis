#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int thread_start(){
    printf("thread_start\n");
    return 2;
}

int main(){
    pthread_t thread;
    pthread_create(&thread,NULL,(void*)thread_start,NULL);
    sleep(2);
    printf("main\n");
    pthread_join(thread,NULL);
}
