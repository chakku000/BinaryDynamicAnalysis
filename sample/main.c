#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

const char* thread_start(){
    printf("thread_start\n");
    return "yuudachi";
}

int main(){
    void* retval;
    printf("&retval = %p\n",&retval);
    printf("thread_start = %p\n",thread_start);
    pthread_t thread;
    pthread_create(&thread,NULL,(void*)thread_start,NULL);
    sleep(1);
    printf("main\n");
    pthread_join(thread,&retval);
    printf("retval = %s\n",(char*)retval);
}
