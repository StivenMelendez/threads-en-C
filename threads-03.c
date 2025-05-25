#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define hilos 5 // numero de hilos a crear si son fijos

pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER; // MUTEX

int ejecutor(); // CREADOR DE HILOS PARA LA SOLUCION
void finalizador(); // funcion estetica

/* INICIO RUTINAS */

    void* funcion(); 

/* FIN RUTINAS */

/* INICIO VARIABLES GLOBALES */

    int x = 1;
    int error_global = 0;

/* FIN VARIABLES GLOBALES */

/* EN EL MAIN SE MANEJARA LA FINALIZACION DEL EJECUTOR */

    int main(){
        if (ejecutor() == 0){
            finalizador();
            printf("\nPROGRAMA FINALIZADO CON EXITO\n");
            return EXIT_SUCCESS;
        }else{
            finalizador();
            //printf("\nOCURRIO UN ERROR\n");
            perror("\nOCURRIO UN ERROR\n");
            return EXIT_FAILURE;
        }
        return -1;
    }

/* FIN DEL MAIN */

/* FUNCION EJECUTOR DONDE SE CREARAN LOS HILOS PARA LA SOLUCION */

int ejecutor(){
    pthread_t thread[hilos];
    pthread_mutex_init(&mutex, NULL);

    int hilos_creados = 0;
    for(int i = 0; i < hilos; i++){
        if(pthread_create(&thread[i], NULL, &funcion, NULL)){
            printf("Error creating thread [%ld]\n", (long int)pthread_self());
            error_global = 1;
            hilos_creados = i;
            break;
        }
    }

    if(error_global) {
        for(int i = 0; i < hilos_creados; i++){
            pthread_cancel(thread[i]);
        }
        pthread_mutex_destroy(&mutex);
        return 1;
    }

    for(int i = 0; i < hilos; i++){
        if(pthread_join(thread[i], NULL)){
            printf("Error joining thread [%ld]\n", (long int)pthread_self());
            error_global = 1;
            for(int j = i+1; j < hilos; j++){
                pthread_cancel(thread[j]);
            }
            pthread_mutex_destroy(&mutex);
            return 2;
        }
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}

/* FIN DEL EJECUTOR */

/* FUNCIONES QUE CONTIENEN RUTINAS QUE HARAN LOS HILOS */

    void* funcion(){
        for(int i = 1; i<=2; i++){
            pthread_mutex_lock(&mutex);
            printf("Thread: [ %ld ] ## x: [ %d ]\n", (long int)pthread_self(), x);
            x++;
            sleep(1);
            pthread_mutex_unlock(&mutex);
        }
        pthread_exit(0);
    }

/* FIN DE LAS FUNCIONES CON RUTINAS */

/* INICIO FUNCIONES ESTETICAS */
    void finalizador(){
        int total = 20;
        for (int a = 0; a <= total; a++){
            int porcentaje = (a * 100) / total;
            printf("\r[");
            for (int i = 0; i < total; i++) {
                if (i < a) printf("#");
                else printf(" ");
            }
            printf("] %d%%", porcentaje);
            fflush(stdout);
            usleep(100000);
        }
    }
/* FIN FUNCIONES ESTETICAS*/
