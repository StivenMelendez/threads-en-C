#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "threadslib.h"
int x = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* thread();

int main(){
    pthread_t t1, t2;

    if(pthread_create(&t1, NULL, &thread, NULL)){
        printf("Error creating thread 1\n");
        return 1;
    }

    if(pthread_create(&t2, NULL, &thread, NULL)){
        printf("Error creating thread 2\n");
        return 1;
    }

    if(pthread_join(t1, NULL)){
        printf("Error joining thread 1\n");
        return 1;
    }

    if(pthread_join(t2, NULL)){
        printf("Error joining thread 2\n");
        return 1;
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}

void* thread(){
    for(int i = 1; i<=10; i++){
        pthread_mutex_lock(&mutex);
        printf("Thread: [ %ld ] ## x: [ %d ]\n", (long int)pthread_self(), x);
        x++;
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

