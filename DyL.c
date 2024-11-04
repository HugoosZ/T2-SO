#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

float TseriesD;
float TseriesB;  
float StseriesD = 0;
float StseriesB = 0;
float t[] = {0.5, 1.0, 1.5, 2.0};
float TdeCadaProfesorD[6];
float TdeCadaProfesorB[6];
float sumatoriaD = 0;
float sumatoriaB = 0;
int Bool = 0;
int Nsemana = 1;
float auxD = 0;
float auxB = 0;
int Pd = 6;
int Pb = 6;


pthread_mutex_t mutexD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexdebool = PTHREAD_MUTEX_INITIALIZER;

// Función para generar un valor aleatorio del arreglo t[]
float t_serie(void) {
    int indice = rand() % 4;
    return t[indice];
}

// Genera una cantidad aleatoria de series
int c_series(void) {
    int series = rand() % 6 + 10;
    return series;
}

// Función de Deadlock
void *deadlock_function(void *arg) {
    int id = *(int *)arg;
    float Tvisualizacion = t_serie();
    printf("Profesor %d intentando adquirir recursos para deadlock...\n", id);

    if (id < (Pd + 1)) {
        pthread_mutex_lock(&mutexD);
        printf("Profesor Dasney %d adquirió mutexD\n", id);
        sleep(1);
        // Logica de series
        pthread_mutex_lock(&mutexB);
        printf("Profesor Dasney %d adquirió mutexB\n", id);
    } else {
        pthread_mutex_lock(&mutexB);
        printf("Profesor Betflix %d adquirió mutexB\n", id);
        sleep(1);
        // Logica de series
        pthread_mutex_lock(&mutexD);
        printf("Profesor Betflix %d adquirió mutexD\n", id);
    }
    pthread_mutex_unlock(&mutexD);
    pthread_mutex_unlock(&mutexB);


    free(arg);
    return NULL;
}

// Función de Livelock
volatile int trying_dasney = 0;
volatile int trying_betflix = 0;
int contador = 0;
void *livelock_function(void *arg) {
    int id = *(int *)arg;
    float Tvisualizacion = t_serie();
    while (1) {
        if (id < (Pd + 1)) {  // Profesor Dasney
            trying_dasney = 1;
            if (trying_betflix) {
                printf("Profesor Dasney %d cediendo a profesores de Betflix...\n", id);
                sleep(1);
                continue;
            }
            pthread_mutex_lock(&mutexD);
            trying_dasney = 0;
            printf("Profesor Dasney %d en región crítica\n", id);
            // Logica de series
            pthread_mutex_unlock(&mutexD);
            break;
        } else {  // Profesor Betflix
            trying_betflix = 1;
            if (trying_dasney) {
                printf("Profesor Betflix %d cediendo a profesores de Dasney...\n", id);
                sleep(1);
                continue;
            }
            // Procede si el otro no está intentando
            pthread_mutex_lock(&mutexB);
            trying_betflix = 0;
            printf("Profesor Betflix %d en región crítica\n", id);
            // Logica de series
            pthread_mutex_unlock(&mutexB);
            break;
        }
    }
    free(arg);
    return NULL;
}

int main(void) {
    srand((unsigned int)time(NULL));
    pthread_t PD[Pd];  // Profesores que ven Dasney
    pthread_t PB[Pb];  // Profesores que ven Betflix

    int N = 0;
    int Ctiempo = 0;
    int choice = 0;
    
    printf("Seleccione el modo:\n");
    printf("1. Demostración de deadlock\n");
    printf("2. Demostración de livelock\n");
    scanf("%d", &choice);

    void *(*selected_function)(void *);
    if (choice == 1) {
        selected_function = deadlock_function;
    } else if (choice == 2) {
        selected_function = livelock_function;
    }

    printf("Ingrese el tiempo:\n[1] 1 Mes\n[2] 6 Mes\n[3] 1 Año\n");
    scanf("%d", &N);
    if (N == 1) {
        Ctiempo = 4;
    } else if (N == 2) {
        Ctiempo = 24;
    } else if (N == 3) {
        Ctiempo = 48;
    }


    while (Ctiempo > 0) {
        Bool = 0;
        sumatoriaB = 0;
        sumatoriaD = 0;
        TseriesD = c_series();
        TseriesB = c_series();
        
        printf("-------------------------------------------------------\n");
        printf("Semana %d\n", Nsemana);
        printf("-------------------------------------------------------\n");
        printf("Esta semana salieron %.1f series de Dasney\n", TseriesD);
        printf("Esta semana salieron %.1f series de Betflix\n", TseriesB);
        auxB = TseriesB;
        auxD = TseriesD;

        for (int i = 0; i < Pd; i++) {
            int *idD = malloc(sizeof(int));
            *idD = i + 1; 
            pthread_create(&PD[i], NULL, selected_function, (void *)idD);
        }
        for (int i = 0; i < Pb; i++) {
            int *idB = malloc(sizeof(int));  
            *idB = i + Pd + 1;
            pthread_create(&PB[i], NULL, selected_function, (void *)idB);
        }

        for (int i = 0; i < 6; i++) {
            pthread_join(PD[i], NULL);
            pthread_join(PB[i], NULL);
        }
        Ctiempo--;
        Nsemana++;
        TseriesD = 0;
        TseriesB = 0;
    }
    

    return 0;
}