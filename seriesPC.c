#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define NUM_PROFESORES 6

// Variables de series y acumuladores
float TseriesD, TseriesB;
float StseriesD = 0, StseriesB = 0;
float sumatoriaD = 0, sumatoriaB = 0;
int Bool = 0;
int Nsemana = 1;
float TdeCadaProfesorD[NUM_PROFESORES];
float TdeCadaProfesorB[NUM_PROFESORES];

// Semáforos y mutexes para sincronización
sem_t seriesListas; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBool = PTHREAD_MUTEX_INITIALIZER;

// Función que retorna un valor aleatorio del arreglo t[]
float t_serie(void) {
    float t[] = {0.5, 1.0, 1.5, 2.0};
    int indice = rand() % 4;
    return t[indice];
}

// Función que produce series para ver
void producir_series(void) {
    TseriesD = rand() % 6 + 10;
    TseriesB = rand() % 6 + 10;
    printf("-------------------------------------------------------\n");
    printf("Semana %d\n", Nsemana);
    printf("-------------------------------------------------------\n");
    printf("Series disponibles Dasney: %.1f\n", TseriesD);
    printf("Series disponibles Betflix: %.1f\n", TseriesB);
    sem_post(&seriesListas);  // Señala que las series están listas para ver
}

// Función que simula el consumo de series por los profesores
void *consumir_series(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        sem_wait(&seriesListas);  // Espera hasta que haya series disponibles
        pthread_mutex_lock(&mutex);

        float Tvisualizacion = t_serie();
        if (id < NUM_PROFESORES) { // Profesores Dasney
            if (TseriesD >= Tvisualizacion) {
                TseriesD -= Tvisualizacion;
                TdeCadaProfesorD[id] = Tvisualizacion;
                sumatoriaD += Tvisualizacion;
            } else {
                Tvisualizacion = TseriesD;
                TdeCadaProfesorD[id] = Tvisualizacion;
                TseriesD = 0;
            }
        } else { // Profesores Betflix
            if (TseriesB >= Tvisualizacion) {
                TseriesB -= Tvisualizacion;
                TdeCadaProfesorB[id - NUM_PROFESORES] = Tvisualizacion;
                sumatoriaB += Tvisualizacion;
            } else {
                Tvisualizacion = TseriesB;
                TdeCadaProfesorB[id - NUM_PROFESORES] = Tvisualizacion;
                TseriesB = 0;
            }
        }

        pthread_mutex_lock(&mutexBool);
        Bool++;
        pthread_mutex_unlock(&mutexBool);

        // Verificación y reporte semanal
        if (Bool == NUM_PROFESORES * 2) {
            pthread_mutex_unlock(&mutex);
            printf("-------------------------------------------------------\n");
            for (int i = 0; i < NUM_PROFESORES; i++) {
                printf("El profesor %d vio %.1f series de Dasney\n", i + 1, TdeCadaProfesorD[i]);
            }
            for (int i = 0; i < NUM_PROFESORES; i++) {
                printf("El profesor %d vio %.1f series de Betflix\n", i + 1, TdeCadaProfesorB[i]);
            }
            printf("Total de series vistas de Dasney: %.1f\n", sumatoriaD);
            printf("Total de series vistas de Betflix: %.1f\n", sumatoriaB);
            printf("-------------------------------------------------------\n");

            Nsemana++;
            Bool = 0;
            sem_post(&seriesListas);  // Permite continuar la producción
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(void) {
    srand((unsigned int)time(NULL));

    pthread_t PD[NUM_PROFESORES];
    pthread_t PB[NUM_PROFESORES];
    sem_init(&seriesListas, 0, 0);

    // Creación de hilos para cada profesor
    for (int i = 0; i < NUM_PROFESORES; i++) {
        int *idD = malloc(sizeof(int));
        int *idB = malloc(sizeof(int));
        *idD = i;
        *idB = i + NUM_PROFESORES;
        pthread_create(&PD[i], NULL, consumir_series, (void *)idD);
        pthread_create(&PB[i], NULL, consumir_series, (void *)idB);
    }

    // Ciclo de producción de series
    int Ctiempo;
    printf("Ingrese el tiempo en semanas: ");
    scanf("%d", &Ctiempo);

    while (Ctiempo > 0) {
        producir_series();
        Ctiempo--;
    }

    for (int i = 0; i < NUM_PROFESORES; i++) {
        pthread_join(PD[i], NULL);
        pthread_join(PB[i], NULL);
    }

    sem_destroy(&seriesListas);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexBool);

    return 0;
}