/*
 * スレッドを10個作成
 * 各スレッドはなにもしない関数を呼び出す
 */
#include <stdio.h>
#include <pthread.h>

const int N = 10;

void f(){}

int main(){
    pthread_t threads[N];
    int i;
    for(i=0;i<N;i++){
        pthread_create(&threads[i],NULL,(void*)f,NULL);
    }
    for(i=0;i<N;i++){
        pthread_join(threads[i],NULL);
    }
}
