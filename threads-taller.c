/* INICIO PRBLEMA
    Asedio del castillo:
    se requiere una solucion para implementar sistema de asedio a un castillo en un videojuego:
    los castillos son asediados por grupos de infanteria cada grupo esta compuesto por 3000 unidades
    es decir una proporcion de 1:3000, la prueba se realizara sobre un castillo de nivel 4 que ocupa 
    12000 unidades de asedio en total es decir 4 grupos de 3000, un castillo de nivel 4 tiene una guardia
    de 6000 infanterias, esta defiende de manera general el castillo, es decir se enfrentan a los diferentes
    grupos de incursion que asedian el castillo a la vez.
    ATACANTES:
        - la posibilidad de perder el 5% (muertes por atricion) unidades en cada ronda de asedio son de 95%
        - la posibilidad de perder mas del 10% (Causalidades por asedio) unidades por ronda es de 50% luego del calculo anterior
        - la posibilidad de perder un 10% mas de unidades (despistados) es del 10% luego del calculo anterior
        - la posibilidad de avanzar el asedio cada ronda de asedio es de 50%
            + el avance a la caida del castillo aumenta 7%
            + el maximo de avance de caida del castillo es de 100%
        - el grupo de reposicion es de 12000
    DEFENSORES:
        - la posibilidad de perder 10% de la guarnicion es de 5% (atricion) durante las primeras 30 rondas
        - la posibilidad de perder 10% de la guarnicion en defensa es de 10%.
        - la posibilidad de que se desate una enfermedad en el castillo cada 30 rondas de asedio es de 50%
            + si se desata una enfermedad cada 7 rondas la guarnicion se reduce un 3%
        - la posibilidad de perder 10% de la guarnicion luego de 60 rondas aumenta a 45%
        - la posibilidad de retroceder el asedio cada ronda de asedio es de 50%
            + el avance a la caida del castillo disminuye 7%
            + el maximo de avance de caida del castillo es de -21%
    GENERAL:
        - cada ronda que el asedio sea positivo se estimara si e fuerte puede caer o no.
        - la guarnicion no tiene repuesto dado que el castillo esta bajo asedio
        - los asediantes tienen un limite de reposicion de infanteria
FIN PROBLEMA */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NIVEL_CASTILLO 4 //NIVEL DEL CASTILLO DEFENSOR
#define GRUPOS_DE_INFANTERIA 4 // GRUPOS QUE ASEDIARAN EL CASTILLO, NUMERO DE HILOS
#define CANTIDAD_DE_INFANTERIA_POR_GRUPO 3000 //STACKS DE INFANTERIA PROPORCION 1:3000
#define GUARNICION 6000 //DEFENSORES DEL CASTILLO
//#define HILOS 4 //ES IGUAL CANTIDAD DE GRUPOS DE ASEDIO
#define REPOSICION 12000 //CANTIDAD DE REPOSICION


/* INICIO FUNCIONES */

    int solucion_semaforos();
    //void solucion_mutex ();
    //void solucion_barreras();
    void inicializacion(); // sirve para inicializar y resetear.
    int calculador_de_probabilidad(int probabilidad);
    void* asedio_con_semaforos(void* arg);
    int ejecutor(); // CREADOR DE HILOS PARA LA SOLUCION
    void finalizador(); // funcion estetica
    
/* FIN FUNCIONES */

/* INICIO VARIABLES GLOBALES */
    int ASEDIO; //ENTRE -21% Y 100%
    int ATACANTES[GRUPOS_DE_INFANTERIA];
    int DEFENSORES;
    int RESERVAS;
    int RONDAS;
    int id[GRUPOS_DE_INFANTERIA]; //para no usar long int para el id del hilo
    pthread_t HILOS[GRUPOS_DE_INFANTERIA];
    sem_t asediando;
    int estado_castillo = 1;
/* FIN VARIABLES GLOBALES */

int main(){
    if (solucion_semaforos() == 0){
        finalizador();
        printf("\nPROGRAMA FINALIZADO CON EXITO\n");
        return EXIT_SUCCESS;
    }else{
        finalizador();
        perror("\nOCURRIO UN ERROR\n");
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}

void inicializacion(){
    ASEDIO = 0;
    for(int x = 0; x < GRUPOS_DE_INFANTERIA; x++){
        ATACANTES[x] = CANTIDAD_DE_INFANTERIA_POR_GRUPO;
        //HILOS[x] = 0;
    }
    DEFENSORES = GUARNICION;
    RESERVAS = REPOSICION;
}

int calculador_de_probabilidad(int probabilidad){
    int random_num = 0;
    int probabilidad_restante = 100-probabilidad, retorno = 0;

    if(probabilidad < 0 || probabilidad > 100) return -1; //error
    if(probabilidad == 0) return 1; // no se cumple 
    if(probabilidad == 100) return 0; // se cumple

    random_num = rand() % 100 + 1;
    if (random_num <= probabilidad) {
        retorno = 0; // se cumple la probabilidad
    } else {
        retorno = 1; // no se cumple la probabilidad
    }
}

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

int cantidad_infanteria(){
    int cant;
    for(int x = 0; x < GRUPOS_DE_INFANTERIA; x++){
        cant =+ ATACANTES[x];
    }
    return cant;
}

/* SOLUCION CON SEMAFOROS */
    int solucion_semaforos(){
        inicializacion();
        for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++){
            id[i] = i;
            if(pthread_create(&HILOS[i], NULL, &asedio_con_semaforos, &id[i])){
                printf("Error creating thread [%ld]\n", (long int)pthread_self());
            }
        }

        for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++){
            if(pthread_join(HILOS[i], NULL)){
                printf("Error joining thread [%ld]\n", (long int)pthread_self());
            }
        }
        return 1;
    }

    void* asedio_con_semaforos(void* arg){
        int id = *((int*)arg);
        int Aperdidas_acumuladas = 0, Aperdidas_al_momento = 0;
        int Dperdidas_acumuladas = 0, Dperdidas_al_momento = 0;
        while(cantidad_infanteria() >= 12000 && estado_castillo == 1){
            printf("el grupo [%d] esta asediando\n",id);
            printf("TAMAÑO DE LOS GRUPOS DE INFANTERIA [%d]\n", ATACANTES[id]);
            printf("RESERVAS [%d]\n", RESERVAS);
            printf("TAMAÑO DE LA GUARNICION DEFENSIVA: [%d]", DEFENSORES);
            printf("PROCENTAJE DE ASEDIO [%d%%]\n", ASEDIO);
            
            // ¿AVANZA EL ASEDIO?
            if (calculador_de_probabilidad(ASEDIO) == 1){
                if(calculador_de_probabilidad(50) == 0){
                    ASEDIO =+ 7;
                    printf("Nuevo porcentaje de asedio [%d%%]\n", ASEDIO);
                }else if(calculador_de_probabilidad(50) == 0){
                    ASEDIO =- 7;
                    printf("Nuevo porcentaje de asedio [%d%%]\n", ASEDIO);
                }else{
                    printf("status quo [%d%%]\n", ASEDIO);
                }
    
                //¿MUERTES DE INFANTERIA ATACANTE?
                if(calculador_de_probabilidad(95) == 0){//atricion
                    Aperdidas_al_momento = ATACANTES[id] * 0.05;
                    Aperdidas_acumuladas =+ Aperdidas_al_momento;
                    ATACANTES[id] =- Aperdidas_acumuladas;
                    printf("HAN MUERTO [%d] POR ATRICION \n", Aperdidas_acumuladas);
                    Aperdidas_al_momento = 0;
                }
    
                if(calculador_de_probabilidad(50) == 0){//asedio
                    Aperdidas_al_momento = ATACANTES[id] * 0.1;
                    Aperdidas_acumuladas = Aperdidas_al_momento;
                    ATACANTES[id] =- Aperdidas_acumuladas;
                    printf("HAN MUERTO [%d] ASEDIANDO \n", Aperdidas_acumuladas);
                    Aperdidas_al_momento = 0;
                }
    
                if(calculador_de_probabilidad(10) == 0){//despistados
                    Aperdidas_al_momento = ATACANTES[id] * 0.1;
                    Aperdidas_acumuladas = Aperdidas_al_momento;
                    ATACANTES[id] =- Aperdidas_acumuladas;
                    printf("HAN MUERTO [%d] POR MONGOLOS \n", Aperdidas_acumuladas);
                    Aperdidas_al_momento = 0;
                }
    
    
    
                //MUERTES DEL DEFENSOR
                if(calculador_de_probabilidad(10) == 0){ //atricion al defender
                    Dperdidas_al_momento = DEFENSORES * 0.05;
                    DEFENSORES =- Dperdidas_al_momento;
                    printf("HAN MUERTO [%d] POR ATRICION \n", Dperdidas_acumuladas);
                }
                
    
                printf("perdidas totales del atacante [%d]\n", Aperdidas_acumuladas);
    
                if(RESERVAS > 0 && Aperdidas_acumuladas > 0){ //REPONER DESDE LA RESERVA
                    printf("se repondran [%d] de la reserva \n", Aperdidas_acumuladas);
                    if (RESERVAS >= Aperdidas_acumuladas){
                        RESERVAS =- Aperdidas_acumuladas;
                        ATACANTES[id] =+ Aperdidas_acumuladas;
                    }else if ((RESERVAS + ATACANTES[id]) <= 3000){
                        ATACANTES[id] =+ RESERVAS;
                    }
                }

            }else{
                estado_castillo = 0;
            }
            RONDAS++;
        }
        return NULL;
    }
/* FIN DE LA SOLUCION CON SEMAFOROS */