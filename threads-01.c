#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

void* maneja_hilo();

int main(){
    int c = 0;
    pthread_t pidhilo;
    pthread_create(&pidhilo, NULL, maneja_hilo, NULL);
    //printf("hilo principal ( idthread -> [%ld] )\n", (long int) pthread_self());
    while (true)
    {
        printf("[c -> %d]\n", c);
        c++;
        sleep(0);
    }   
    
    return 0;
}

void* maneja_hilo(){
    int p = 0;
    //printf("hilo( idthread -> [%ld] )\n", (long int) pthread_self());
    while (true)
    {
        printf("[p -> %d]\n", p);
        p++;
        sleep(0);
    }   
    pthread_exit(NULL);
}