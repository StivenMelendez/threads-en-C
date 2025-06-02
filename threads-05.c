/*Barbero Dormilon*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_SILLAS 3
#define NUM_CLIENTES 10

sem_t barbero_listo, clientes_esperando;
pthread_mutex_t atencion = PTHREAD_MUTEX_INITIALIZER;
int sillas_libres = NUM_SILLAS;

void* barbero(void* args);
void* cliente(void*args);

int main(int arg, char** args){

    pthread_t barbero;
    pthread_t clientes[NUM_CLIENTES];
    sem_init(&barbero_listo, 0, NULL);
    sem_init(&clientes_esperando, 0, NULL);

    if(!pthead_create(&barbero, barbero, NULL)){
        printf("Error al crear hilo\n");
    }


    return EXIT_SUCCESS;
}

void* barbero(void*args){
    while (1){
        sem_wait(&clientes_esperando);
        pthread_mutex_lock(&atencion);
        sillas_libres++;
        printf("se libero una silla se atendera un cliente\n");
        printf("sillas libres %d", sillas_libres);
        pthread_mutex_unlock(&atencion);
    }
}

void* cliente(void*args){
    int* idx = *(int*)args;
    sleep(rand() % 3);
    pthread_mutex_lock(&atencion);
    if (sillas_libres >= 1){
        printf("el cliente se puede sentar");
        sillas_libres--;
        pthread_mutex_unlock(&atencion);
        sem_post(&clientes_esperando);
        sem_wait(&barbero_listo);
    }else{
        printf("no hay sillas disponibles\n");
        pthread_mutex_unlock(&atencion);
    }
}