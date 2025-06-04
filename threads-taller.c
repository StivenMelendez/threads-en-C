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
            + el minimo de avance de caida del castillo es de 0%
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

#define NIVEL_CASTILLO 4
#define GRUPOS_DE_INFANTERIA 4
#define CANTIDAD_DE_INFANTERIA_POR_GRUPO 3000
#define GUARNICION 6000
#define REPOSICION 4000

/* INICIO FUNCIONES */
int solucion_semaforos();
int solucion_mutex();
int solucion_espera_activa();
void inicializacion();
int calculador_de_probabilidad(int probabilidad);
void* asedio_con_semaforos(void* arg);
void* asedio_con_mutex(void* arg);
void* asedio_con_espera_activa(void* arg);
int cantidad_infanteria();
void finalizador();
void comparar_metodos();
long para_medir_tiempo(long in, long fi);
long para_hacer_promedio(long acum, int ro);
/* FIN FUNCIONES */

/* INICIO VARIABLES GLOBALES */
int ASEDIO;
int ATACANTES[GRUPOS_DE_INFANTERIA];
int DEFENSORES;
int RESERVAS;
int RONDAS;
int id[GRUPOS_DE_INFANTERIA];
int estado_castillo;
int enfermedad;
int turno;
int bandera_ocupado;
int espera_apagada;
int rmu, rse, rea;
int mej_mu, mej_se, mej_ea;
long rapnue;
pthread_t HILOS[GRUPOS_DE_INFANTERIA];
sem_t asediando;
pthread_mutex_t mutex_asedio;
pthread_barrier_t barrera_asedio;

typedef struct{long inicio, fin, acum; double prom;}medicion;

medicion 
para_mutex = {0, 0, 0, 0}, 
para_semaforo = {0, 0, 0, 0}, 
para_espera_activa = {0, 0, 0, 0};

/* FIN VARIABLES GLOBALES */

long mejor_tiempo_semaforo;
long mejor_tiempo_mutex;
long mejor_tiempo_espera_activa;

int main() {
    srand(time(NULL));
    while (1){
        rmu = 0;
        rse = 0;
        rea = 0;
        mejor_tiempo_semaforo = __LONG_MAX__;
        mejor_tiempo_mutex = __LONG_MAX__;
        mejor_tiempo_espera_activa = __LONG_MAX__;
        printf("\n\n=== COMENZANDO SIMULACION CON SEMAFOROS ===\n\n");
        if (solucion_semaforos() == 0) {
            finalizador();
            printf("\nSIMULACION CON SEMAFOROS FINALIZADA CON ÉXITO\n");
        } else {
            finalizador();
            printf("\nERROR EN SIMULACION CON SEMAFOROS\n");
        }
        //sleep(5);    
        printf("\n\n=== COMENZANDO SIMULACION CON ESPERA ACTIVA ===\n\n");
        if (solucion_espera_activa() == 0) {
            finalizador();
            printf("\nSIMULACION CON ESPERA ACTIVA FINALIZADA CON ÉXITO\n");
        } else {
            finalizador();
            printf("\nERROR EN SIMULACION CON ESPERA ACTIVA\n");
        }
        //sleep(5);
        printf("\n\n=== COMENZANDO SIMULACION CON MUTEX ===\n\n");
        if (solucion_mutex() == 0) {
            finalizador();
            printf("\nSIMULACION CON MUTEX FINALIZADA CON ÉXITO\n");
        } else {
            finalizador();
            printf("\nERROR EN SIMULACION CON MUTEX\n");
        }

        comparar_metodos();
        printf("presione ENTER para continuar:");
        getchar();
    }
    return EXIT_SUCCESS;
}

void inicializacion() {
    for (int x = 0; x < GRUPOS_DE_INFANTERIA; x++) {
        ATACANTES[x] = CANTIDAD_DE_INFANTERIA_POR_GRUPO;
    }
    ASEDIO = 0;
    DEFENSORES = GUARNICION;
    RESERVAS = REPOSICION;
    RONDAS = 0;
    estado_castillo = 1;
    enfermedad = 1;
    turno = 0;
    bandera_ocupado = 0;
    espera_apagada = 1;
    rapnue = 0;
}
long para_medir_tiempo(long in, long fi){
    return (fi - in);
}
long para_hacer_promedio(long acum, int ro){
    return ((double)acum/(double)ro);
}
int calculador_de_probabilidad(int probabilidad) {
    if (probabilidad < 0 || probabilidad > 100) return -1;
    if (probabilidad == 0) return 1;
    if (probabilidad == 100) return 0;

    int random_num = rand() % 100 + 1;
    return (random_num <= probabilidad) ? 0 : 1;
}
int cantidad_infanteria() {
    int cant = 0;
    for (int x = 0; x < GRUPOS_DE_INFANTERIA; x++) {
        cant += ATACANTES[x];
    }
    return cant;
}
void finalizador() {
    int total = 20;
    for (int a = 0; a <= total; a++) {
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

void registrar_mejor_tiempo(long tiempo_actual, int metodo) {
    switch (metodo) {
        case 1: // Semáforos
            if (tiempo_actual < mejor_tiempo_semaforo && tiempo_actual > 0)
                mejor_tiempo_semaforo = tiempo_actual;
            break;
        case 2: // Mutex
            if (tiempo_actual < mejor_tiempo_mutex && tiempo_actual > 0)
                mejor_tiempo_mutex = tiempo_actual;
            break;
        case 3: // Espera activa
            if (tiempo_actual < mejor_tiempo_espera_activa && tiempo_actual > 0)
                mejor_tiempo_espera_activa = tiempo_actual;
            break;
    }
}

void comparar_metodos() {
    printf("\n\n***** COMPARACION DE RENDIMIENTO *****\n");
    printf("Metodo         || Mejor Tiempo (ms)      || RONDAS\n");
    printf("***************||***************************|| \n");
    printf("SEMAFOROS      || [%.5f]**************|| [ %d ]\n", mejor_tiempo_semaforo / 1000000.0, rse);
    printf("MUTEX          || [%.5f]**************|| [ %d ]\n", mejor_tiempo_mutex / 1000000.0, rmu);
    printf("ESPERA ACTIVA  || [%.5f]**************|| [ %d ]\n", mejor_tiempo_espera_activa / 1000000.0, rea);
    printf("**********************************************\n\n");
    
    // Determinar el método más rápido basado en el mejor tiempo
    if (mejor_tiempo_semaforo <= mejor_tiempo_mutex && mejor_tiempo_semaforo <= mejor_tiempo_espera_activa) {
        printf("El metodo mas rapido es: SEMAFOROS\n");
    }
    else if (mejor_tiempo_mutex <= mejor_tiempo_semaforo && mejor_tiempo_mutex <= mejor_tiempo_espera_activa) {
        printf("El metodo mas rapido es: MUTEX\n");
    }
    else {
        printf("El metodo mas rapido es: ESPERA ACTIVA\n");
    }
    
    /*printf("\nMejores tiempos en milisegundos:\n");
    printf("SEMAFOROS: %.5f ms\n", mejor_tiempo_semaforo / 1000000.0);
    printf("MUTEX: %.5f ms\n", mejor_tiempo_mutex / 1000000.0);
    printf("ESPERA ACTIVA: %.5f ms\n", mejor_tiempo_espera_activa / 1000000.0);*/
}

/* SOLUCION CON SEMAFOROS */
int solucion_semaforos() {
    inicializacion();
    sem_init(&asediando, 0, 1);
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        id[i] = i;
        if (pthread_create(&HILOS[i], NULL, &asedio_con_semaforos, &id[i])) {
            printf("Error creando hilo [%d]\n", i);
        }
    }
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        if (pthread_join(HILOS[i], NULL)) {
            printf("Error esperando hilo [%d]\n", i);
        }
    }
    para_semaforo.prom = para_hacer_promedio(para_semaforo.acum, RONDAS);
    rse = RONDAS;
    mej_se = rapnue;
    printf("\nlos resultados de la medicion para [SEMAFORO] es: \n");
    printf("el promedio con:\n Rondas:[ %d ]\n El promedio es:[ %.2f ] nanosegundos\n", RONDAS, para_semaforo.prom);
    /*printf("presione ENTER para continuar:");
    getchar();*/
    sem_destroy(&asediando);
    return 0;
}
void* asedio_con_semaforos(void* arg) {
    int id = *((int*)arg);
    int Aperdidas, Dperdidas;
    struct timespec tiempo_inicio, tiempo_fin;
    
    while (1) {
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20) {
            return NULL;
        }
        sleep(1);
        
        // Capturar tiempo justo antes de intentar adquirir el semaforo
        clock_gettime(CLOCK_MONOTONIC, &tiempo_inicio);
        
        sem_wait(&asediando); // INICIO DE LA SECCION CRITICA
        
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20) {
            sem_post(&asediando);
            return NULL;
        }
        printf("\n[SEMAFOROS] El grupo [%d] esta asediando\n", id);
        printf("Infa actual: [%d]\n", ATACANTES[id]);
        printf("Guarnicion actual: [%d]\n", DEFENSORES);
        printf("Reservas: [%d], Ronda: [%d], Asedio: [%d%%]\n", RESERVAS, RONDAS, ASEDIO);
        
        if (calculador_de_probabilidad(ASEDIO) == 1) {
            if (calculador_de_probabilidad(50) == 0) {
                ASEDIO += 7;
                if (ASEDIO > 100) ASEDIO = 100;
                printf("¡Asedio avanza! Nuevo nivel: %d%%\n", ASEDIO);
            } else if (calculador_de_probabilidad(50) == 0) {
                ASEDIO -= 7;
                if (ASEDIO < 0) ASEDIO = 0;
                printf("¡Asedio retrocede! Nuevo nivel: %d%%\n", ASEDIO);
            } else {
                printf("El asedio no cambia esta ronda.\n");
            }
            Aperdidas = 0;
            if (calculador_de_probabilidad(95) == 0) {
                Aperdidas += ATACANTES[id] * 0.05;
                printf("Bajas por atricion: %d\n", Aperdidas);
            }
            if (calculador_de_probabilidad(50) == 0) {
                Aperdidas += ATACANTES[id] * 0.10;
                printf("Bajas por asedio directo: %d\n", Aperdidas - (int)(ATACANTES[id] * 0.05));
            }
            if (calculador_de_probabilidad(10) == 0) {
                Aperdidas += ATACANTES[id] * 0.10;
                printf("Bajas por desorganizacion: %d\n", Aperdidas - (int)(ATACANTES[id] * 0.15));
            }
            ATACANTES[id] -= Aperdidas;
            if (ATACANTES[id] < 0) ATACANTES[id] = 0;
            Dperdidas = 0;
            if (calculador_de_probabilidad(5) == 0) {
                Dperdidas += DEFENSORES * 0.10;
                printf("Bajas por atricion (defensores): %d\n", (int)(DEFENSORES * 0.10));
            }
            if (calculador_de_probabilidad(10) == 0) {
                Dperdidas += DEFENSORES * 0.10;
                printf("Bajas por defensa directa: %d\n", (int)(DEFENSORES * 0.10));
            }
            if (enfermedad == 1 && RONDAS % 30 == 0 && RONDAS != 0 && calculador_de_probabilidad(50) == 0) {
                printf("¡Enfermedad en el castillo!\n");
                enfermedad = 0;
            }
            if (enfermedad == 0 && RONDAS % 7 == 0) {
                Dperdidas += DEFENSORES * 0.03;
                printf("Bajas por enfermedad: %d\n", (int)(DEFENSORES * 0.03));
            }
            DEFENSORES -= Dperdidas;
            if (DEFENSORES < 0) DEFENSORES = 0;
            if (RESERVAS > 0 && Aperdidas > 0) {
                int reposicion = (RESERVAS >= Aperdidas) ? Aperdidas : RESERVAS;
                ATACANTES[id] += reposicion;
                RESERVAS -= reposicion;
                printf("Se reponen %d unidades desde la reserva.\n", reposicion);
            }
        }else {
            printf("\n¡[SEMAFOROS] EL CASTILLO HA CAIDO!\n");
            printf("El castillo do en la ronda %d con un nivel de asedio del %d%%\n", RONDAS, ASEDIO);
            printf("Infa restante: %d\n", cantidad_infanteria());
            printf("Defensores restantes: %d\n", DEFENSORES);
            printf("Reservas restantes: %d\n", RESERVAS);
            estado_castillo = 0;
            sem_post(&asediando);
            return NULL;
        }
        if (DEFENSORES <= GUARNICION * 0.20) {
            printf("\n[SEMAFORO] ¡LA GUARNICION HA SIDO DIEZMADA!\n");
            sem_post(&asediando);
            return NULL;
        }
        if (cantidad_infanteria() < 12000) {
            printf("\n[SEMAFORO] ¡EL ASEDIO SE HA PERDIDO POR FALTA DE TROPAS!\n");
            printf("Tropas totales: [%dnimo necesario: 12000\n", cantidad_infanteria());
            estado_castillo = 0;
            sem_post(&asediando);//FIN DE LA SECCION CRTIICA
            return NULL;
        }
        RONDAS++;
        sem_post(&asediando); // FIN DE LA SECCION CRITICA
        
        // Capturar tiempo justo despues de liberar el semaforo
        clock_gettime(CLOCK_MONOTONIC, &tiempo_fin);
        
        // Calcular tiempo en nanosegundos y registrar el mejor tiempo
        para_semaforo.inicio = tiempo_inicio.tv_sec * 1000000000L + tiempo_inicio.tv_nsec;
        para_semaforo.fin = tiempo_fin.tv_sec * 1000000000L + tiempo_fin.tv_nsec;
        long tiempo_actual = para_medir_tiempo(para_semaforo.inicio, para_semaforo.fin);
        
        // Registrar el mejor tiempo para semáforos (método 1)
        registrar_mejor_tiempo(tiempo_actual, 1);
        
        para_semaforo.acum += tiempo_actual;
    }
    
    return NULL;
}
/* SOLUCION CON MUTEX */
int solucion_mutex() {
    inicializacion();
    pthread_mutex_init(&mutex_asedio, NULL);
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        id[i] = i;
        if (pthread_create(&HILOS[i], NULL, &asedio_con_mutex, &id[i])) {
            printf("Error creando hilo [%d]\n", i);
        }
    }
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        if (pthread_join(HILOS[i], NULL)) {
            printf("Error esperando hilo [%d]\n", i);
        }
    }

    para_mutex.prom = para_hacer_promedio(para_mutex.acum, RONDAS);
    rmu = RONDAS;
    mej_mu = rapnue;
    printf("\nlos resultados de la medicion para [MUTEX] es: \n");
    printf("el promedio con:\n Rondas:[ %d ]\n El promedio es:[ %.2f ] nanosegundos\n", RONDAS, para_mutex.prom);
    /*printf("presione ENTER para continuar:");
    getchar();*/
    pthread_mutex_destroy(&mutex_asedio);
    return 0;
}
void* asedio_con_mutex(void* arg){
    int id = *((int*)arg);
    int Aperdidas, Dperdidas;
    struct timespec tiempo_inicio, tiempo_fin;
    
    while (1) {
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20 || RONDAS >= 30) {
            return NULL;
        }
        sleep(1);
        
        
        clock_gettime(CLOCK_MONOTONIC, &tiempo_inicio);
        
        pthread_mutex_lock(&mutex_asedio); 
        
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20 || RONDAS >= 30) {
            pthread_mutex_unlock(&mutex_asedio);
            return NULL;
        }
        
        printf("\n[MUTEX] El grupo [%d] esta asediando\n", id);
        printf("Infa actual: [%d]\n", ATACANTES[id]);
        printf("Guarnicion actual: [%d]\n", DEFENSORES);
        printf("Reservas: [%d], Ronda: [%d], Asedio: [%d%%]\n", RESERVAS, RONDAS, ASEDIO);
        
        if (calculador_de_probabilidad(ASEDIO) == 1) {
            if (calculador_de_probabilidad(50) == 0) {
                ASEDIO += 7;
                if (ASEDIO > 100) ASEDIO = 100;
                printf("¡Asedio avanza! Nuevo nivel: %d%%\n", ASEDIO);
            } else if (calculador_de_probabilidad(50) == 0) {
                ASEDIO -= 7;
                if (ASEDIO < 0) ASEDIO = 0;
                printf("¡Asedio retrocede! Nuevo nivel: %d%%\n", ASEDIO);
            } else {
                printf("El asedio no cambia esta ronda.\n");
            }
            Aperdidas = 0;
            if (calculador_de_probabilidad(95) == 0) {
                Aperdidas += ATACANTES[id] * 0.05;
                printf("Bajas por atricion: %d\n", Aperdidas);
            }
            if (calculador_de_probabilidad(50) == 0) {
                int bajas_asedio = ATACANTES[id] * 0.10;
                Aperdidas += bajas_asedio;
                printf("Bajas por asedio directo: %d\n", bajas_asedio);
            }
            if (calculador_de_probabilidad(10) == 0) {
                int bajas_desorg = ATACANTES[id] * 0.10;
                Aperdidas += bajas_desorg;
                printf("Bajas por desorganizacion: %d\n", bajas_desorg);
            }
            ATACANTES[id] -= Aperdidas;
            if (ATACANTES[id] < 0) ATACANTES[id] = 0;
            Dperdidas = 0;
            if (calculador_de_probabilidad(5) == 0) {
                int bajas_def = DEFENSORES * 0.10;
                Dperdidas += bajas_def;
                printf("Bajas por atricion (defensores): %d\n", bajas_def);
            }
            if (calculador_de_probabilidad(10) == 0) {
                int bajas_combate = DEFENSORES * 0.10;
                Dperdidas += bajas_combate;
                printf("Bajas por defensa directa: %d\n", bajas_combate);
            }
            if (enfermedad == 1 && RONDAS % 30 == 0 && RONDAS != 0 && calculador_de_probabilidad(50) == 0) {
                printf("¡Enfermedad en el castillo!\n");
                enfermedad = 0;
            }
            if (enfermedad == 0 && RONDAS % 7 == 0) {
                int bajas_enfermedad = DEFENSORES * 0.03;
                Dperdidas += bajas_enfermedad;
                printf("Bajas por enfermedad: %d\n", bajas_enfermedad);
            }
            DEFENSORES -= Dperdidas;
            if (DEFENSORES < 0) DEFENSORES = 0;
            if (RESERVAS > 0 && Aperdidas > 0) {
                int reposicion = (RESERVAS >= Aperdidas) ? Aperdidas : RESERVAS;
                ATACANTES[id] += reposicion;
                RESERVAS -= reposicion;
                printf("Se reponen %d unidades desde la reserva.\n", reposicion);
            }
        } else {
            estado_castillo = 0;
            printf("\n¡[MUTEX] EL CASTILLO HA CAIDO!\n");
            printf("El castillo do en la ronda %d con un nivel de asedio del %d%%\n", RONDAS, ASEDIO);
            printf("Infanteria restante: %d\n", cantidad_infanteria());
            printf("Defensores restantes: %d\n", DEFENSORES);
            pthread_mutex_unlock(&mutex_asedio);
            return NULL;
        }
        
        if (cantidad_infanteria() < 12000) {
            printf("\n[MUTEX] ¡EL ASEDIO SE HA PERDIDO POR FALTA DE TROPAS!\n");
            printf("Tropas totales: [%d] pero elminimo necesario: 12000\n", cantidad_infanteria());
            estado_castillo = 0;
            pthread_mutex_unlock(&mutex_asedio);
            return NULL;
        }
        if (DEFENSORES <= GUARNICION * 0.20) {
            printf("\n[MUTEX] ¡LA GUARNICION HA SIDO DIEZMADA!\n");
            pthread_mutex_unlock(&mutex_asedio);
            return NULL;
        }
        RONDAS++;
        pthread_mutex_unlock(&mutex_asedio);
        
        
        clock_gettime(CLOCK_MONOTONIC, &tiempo_fin);
        
        // Calcular tiempo en nanosegundos y registrar el mejor tiempo
        para_mutex.inicio = tiempo_inicio.tv_sec * 1000000000L + tiempo_inicio.tv_nsec;
        para_mutex.fin = tiempo_fin.tv_sec * 1000000000L + tiempo_fin.tv_nsec;
        long tiempo_actual = para_medir_tiempo(para_mutex.inicio, para_mutex.fin);
        
        // Registrar el mejor tiempo para mutex (método 2)
        registrar_mejor_tiempo(tiempo_actual, 2);
        
        para_mutex.acum += para_medir_tiempo(para_mutex.inicio, para_mutex.fin);
    }
    
    return NULL;
}
/* SOLUCION CON ESPERA ACTIVA */
int solucion_espera_activa() {
    inicializacion();
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        id[i] = i;
        if (pthread_create(&HILOS[i], NULL, &asedio_con_espera_activa, &id[i])) {
            printf("Error creando hilo [%d]\n", i);
            espera_apagada = 0; 
            return 1;
        }
    }
    
    for (int i = 0; i < GRUPOS_DE_INFANTERIA; i++) {
        if (pthread_join(HILOS[i], NULL)) {
            printf("Error esperando hilo [%d]\n", i);
            return 1;
        }
    }
    para_espera_activa.prom = para_hacer_promedio(para_espera_activa.acum, RONDAS);
    rea = RONDAS;
    mej_ea = rapnue;
    printf("\nlos resultados de la medicion para [ESPERA ACTIVA] es: \n");
    printf("el promedio con:\n Rondas:[ %d ]\n El promedio es:[ %.2f ] nanosegundos\n", RONDAS, para_espera_activa.prom);
    /*printf("presione ENTER para continuar:");
    getchar();*/
    return 0;
}
void* asedio_con_espera_activa(void* arg) {
    int id = *((int*)arg);
    int Aperdidas, Dperdidas;
    struct timespec tiempo_inicio, tiempo_fin;
    
    while (1) {
        
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20 || RONDAS >= 30 || espera_apagada == 0) {
            return NULL;
        }
        
        
        while (turno != id || bandera_ocupado == 1) {
            
            if (estado_castillo == 0 || cantidad_infanteria() < 12000 || 
                DEFENSORES <= GUARNICION * 0.20 || RONDAS >= 30 || espera_apagada == 0) {
                return NULL;
            }
            usleep(10000); 
        }
        
        
        clock_gettime(CLOCK_MONOTONIC, &tiempo_inicio);
        
        
        bandera_ocupado = 1; // INICIO DE LA SECCION CRITICA
        
        
        if (cantidad_infanteria() < 12000 || estado_castillo == 0 || 
            DEFENSORES <= GUARNICION * 0.20 || RONDAS >= 30 || espera_apagada == 0) {
            bandera_ocupado = 0;
            turno = (turno + 1) % GRUPOS_DE_INFANTERIA;
            return NULL;
        }
        
        printf("\n[ESPERA ACTIVA] El grupo [%d] está asediando\n", id);
        printf("Infantería actual: [%d]\n", ATACANTES[id]);
        printf("Guarnición actual: [%d]\n", DEFENSORES);
        printf("Reservas: [%d], Ronda: [%d], Asedio: [%d%%]\n", RESERVAS, RONDAS, ASEDIO);
        
        if (calculador_de_probabilidad(ASEDIO) == 1) {
            if (calculador_de_probabilidad(50) == 0) {
                ASEDIO += 7;
                if (ASEDIO > 100) ASEDIO = 100;
                printf("¡Asedio avanza! Nuevo nivel: %d%%\n", ASEDIO);
            } else if (calculador_de_probabilidad(50) == 0) {
                ASEDIO -= 7;
                if (ASEDIO < 0) ASEDIO = 0;
                printf("¡Asedio retrocede! Nuevo nivel: %d%%\n", ASEDIO);
            } else {
                printf("El asedio no cambia esta ronda.\n");
            }
            
            Aperdidas = 0;
            if (calculador_de_probabilidad(95) == 0) {
                Aperdidas += ATACANTES[id] * 0.05;
                printf("Bajas por atrición del atacante: %d\n", Aperdidas);
            }
            if (calculador_de_probabilidad(50) == 0) {
                int bajas_asedio = ATACANTES[id] * 0.10;
                Aperdidas += bajas_asedio;
                printf("Bajas por asedio: %d\n", bajas_asedio);
            }
            if (calculador_de_probabilidad(10) == 0) {
                int bajas_desorg = ATACANTES[id] * 0.10;
                Aperdidas += bajas_desorg;
                printf("Bajas por desorganización: %d\n", bajas_desorg);
            }
            
            ATACANTES[id] -= Aperdidas;
            if (ATACANTES[id] < 0) ATACANTES[id] = 0;
            
            Dperdidas = 0;
            if (calculador_de_probabilidad(5) == 0) {
                int bajas_def = DEFENSORES * 0.10;
                Dperdidas += bajas_def;
                printf("Bajas por atrición del defensor: %d\n", bajas_def);
            }
            if (calculador_de_probabilidad(10) == 0) {
                int bajas_combate = DEFENSORES * 0.10;
                Dperdidas += bajas_combate;
                printf("Bajas del defensor: %d\n", bajas_combate);
            }
            if (enfermedad == 1 && RONDAS % 30 == 0 && RONDAS != 0 && calculador_de_probabilidad(50) == 0) {
                printf("¡Enfermedad en el castillo!\n");
                enfermedad = 0;
            }
            if (enfermedad == 0 && RONDAS % 7 == 0) {
                int bajas_enfermedad = DEFENSORES * 0.03;
                Dperdidas += bajas_enfermedad;
                printf("Bajas del defensor por enfermedad: %d\n", bajas_enfermedad);
            }
            
            DEFENSORES -= Dperdidas;
            if (DEFENSORES < 0) DEFENSORES = 0;
            
            if (RESERVAS > 0 && Aperdidas > 0) {
                int reposicion = (RESERVAS >= Aperdidas) ? Aperdidas : RESERVAS;
                ATACANTES[id] += reposicion;
                RESERVAS -= reposicion;
                printf("Se reponen %d unidades desde la reserva.\n", reposicion);
            }
        } else {
            printf("\n¡[ESPERA ACTIVA] EL CASTILLO HA CAIDO!\n");
            printf("El castillo ha caído en la ronda %d con un nivel de asedio del %d%%\n", RONDAS, ASEDIO);
            printf("Infantería restante: %d\n", cantidad_infanteria());
            printf("Defensores restantes: %d\n", DEFENSORES);
            
            
            estado_castillo = 0;
            espera_apagada = 0;
            bandera_ocupado = 0;
            
            return NULL;
        }
        
        if (cantidad_infanteria() < 12000) {
            printf("\n[ESPERA ACTIVA] ¡EL ASEDIO SE HA PERDIDO POR FALTA DE TROPAS!\n");
            printf("Tropas totales: [%d] - Mínimo necesario: 12000\n", cantidad_infanteria());
            estado_castillo = 0;
            espera_apagada = 0;
            bandera_ocupado = 0;
            
            return NULL;
        }
        
        if (DEFENSORES <= GUARNICION * 0.20) {
            printf("\n[ESPERA ACTIVA] ¡LA GUARNICIÓN HA SIDO DIEZMADA!\n");
            espera_apagada = 0;
            bandera_ocupado = 0;
            
            return NULL;
        }
        
        RONDAS++;
        turno = (turno + 1) % GRUPOS_DE_INFANTERIA;
        bandera_ocupado = 0; // FIN DE LA SECCION CRITICA
        
        
        clock_gettime(CLOCK_MONOTONIC, &tiempo_fin);
        
        
        // Calcular tiempo en nanosegundos y registrar el mejor tiempo
        para_espera_activa.inicio = tiempo_inicio.tv_sec * 1000000000L + tiempo_inicio.tv_nsec;
        para_espera_activa.fin = tiempo_fin.tv_sec * 1000000000L + tiempo_fin.tv_nsec;
        long tiempo_actual = para_medir_tiempo(para_espera_activa.inicio, para_espera_activa.fin);
        
        // Registrar el mejor tiempo para espera activa (método 3)
        registrar_mejor_tiempo(tiempo_actual, 3);
        
        para_espera_activa.acum += tiempo_actual;
        
        
        usleep(10000);
    }
    
    return NULL;
}