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
float TD = 0;
float TB = 0;
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
sem_t semaforoD;
sem_t semaforoB;

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

    if (id < 7) {
        pthread_mutex_lock(&mutexD);
        printf("Profesor Dasney %d adquirió mutexD\n", id);
        sleep(1);
        pthread_mutex_lock(&mutexB);
        printf("Profesor Dasney %d adquirió mutexB\n", id);
    } else {
        pthread_mutex_lock(&mutexB);
        printf("Profesor Betflix %d adquirió mutexB\n", id);
        sleep(1);
        pthread_mutex_lock(&mutexD);
        printf("Profesor Betflix %d adquirió mutexD\n", id);
    }


    //Implementacion de logica de codigo correcto que de todas formas no se usará porque no se puede salir del deadlock
    if(id < (Pd + 1)){
        if(TseriesD >= Tvisualizacion){
            TseriesD = TseriesD - Tvisualizacion;
            TdeCadaProfesorD[id - 1] =  Tvisualizacion;
        }
        else{
            Tvisualizacion = TseriesD;
            TdeCadaProfesorD[id - 1] =  Tvisualizacion;
        }

    }else {

        if (TseriesB > Tvisualizacion){
            TseriesB = TseriesB - Tvisualizacion;
            TdeCadaProfesorB[id - (Pd + 1)] =  Tvisualizacion;
        }
        else{
            Tvisualizacion = TseriesB;
            TdeCadaProfesorB[id - (Pd + 1)] =  Tvisualizacion;
        }

    }   

    pthread_mutex_unlock(&mutexD);
    pthread_mutex_unlock(&mutexB);
    pthread_mutex_lock(&mutexdebool);
    Bool++;
    pthread_mutex_unlock(&mutexdebool);

    if(Bool == (Pd + Pb)){
        if(Nsemana > 1){
            if(Nsemana == 2){
                printf("-------------------------------------------------------\n");
                printf("Series acumuladas faltantes por ver de la semana pasada de Dasney: %.1f\n", StseriesD);
                printf("Series acumuladas faltantes por ver de la semana pasada de Betflix: %.1f\n", StseriesB);
                printf("-------------------------------------------------------\n");
            }
            else{
                printf("-------------------------------------------------------\n");
                printf("Series acumuladas faltantes por ver de las semanas pasadas de Dasney: %.1f\n", StseriesD);
                printf("Series acumuladas faltantes por ver de las semanas pasadas de Betflix: %.1f\n", StseriesB);
                printf("-------------------------------------------------------\n");
            }
            auxD = auxD + StseriesD  ; 
            auxB = auxB + StseriesB ;

            printf("Series acumuladas por ver Dasney: %.1f\n", auxD);
            printf("Series acumuladas por ver Netflix: %.1f\n", auxB);

        }

        printf("-------------------------------------------------------\n");
        for(int i = 0; i < Pd; i++){
            printf("El profesor %d vio %.1f series de Dasney\n", i + 1, TdeCadaProfesorD[i]);
        }
        printf("-------------------------------------------------------\n");
        for (int i = 0; i < Pb; i++){
            printf("El profesor %d vio %.1f series de Betflix\n", i + 1, TdeCadaProfesorB[i]);
        }
        printf("-------------------------------------------------------\n");
        for(int i = 0; i < Pd; i++){
            sumatoriaD += TdeCadaProfesorD[i];
        }
        for(int i = 0; i < Pb; i++){
            sumatoriaB += TdeCadaProfesorB[i];
        }
        printf("Los profesores vieron %.1f series de Dasney\n", sumatoriaD);
        printf("Los profesores vieron %.1f series de Betflix\n", sumatoriaB);
        printf("-------------------------------------------------------\n");
        StseriesD += TseriesD;
        StseriesB += TseriesB;

        
        printf("Quedan %.1f series de totales de Dasney por ver\n", StseriesD);
        printf("Quedan %.1f series de totales de Betflix por ver\n", StseriesB);


    }
    free(arg);
    return NULL;
}

// Función de Livelock
volatile int trying_dasney = 1;
volatile int trying_betflix = 1;
void *livelock_function(void *arg) {
    int id = *(int *)arg;
    float Tvisualizacion = t_serie();
    
    while (1) {
        if (id < 7) {  // Profesor Dasney
            trying_dasney = 1;
            if (trying_betflix) {
                printf("Profesor Dasney %d cediendo a profesores de Betflix...\n", id);
                sleep(1);
                continue;
            }
            // Procede si el otro no está intentando
            pthread_mutex_lock(&mutexD);
            trying_dasney = 0;
            printf("Profesor Dasney %d en región crítica\n", id);
            if (TseriesD >= Tvisualizacion) {
                    TseriesD -= Tvisualizacion;
                    TdeCadaProfesorD[id - 1] = Tvisualizacion;
                } else {
                    Tvisualizacion = TseriesD;
                    TdeCadaProfesorD[id - 1] = Tvisualizacion;
                    TseriesD = 0;
                }
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
            if (TseriesB >= Tvisualizacion) {
                    TseriesB -= Tvisualizacion;
                    TdeCadaProfesorB[id - 7] = Tvisualizacion;
                } else {
                    Tvisualizacion = TseriesB;
                    TdeCadaProfesorB[id - 7] = Tvisualizacion;
                    TseriesB = 0;
                }
            pthread_mutex_unlock(&mutexB);
            break;
        }
        pthread_mutex_lock(&mutexdebool);
        Bool++;
        pthread_mutex_unlock(&mutexdebool);
        if(Bool == (Pd + Pb)){
            if(Nsemana > 1){
                if(Nsemana == 2){
                    printf("-------------------------------------------------------\n");
                    printf("Series acumuladas faltantes por ver de la semana pasada de Dasney: %.1f\n", StseriesD);
                    printf("Series acumuladas faltantes por ver de la semana pasada de Betflix: %.1f\n", StseriesB);
                    printf("-------------------------------------------------------\n");
                }
                else{
                    printf("-------------------------------------------------------\n");
                    printf("Series acumuladas faltantes por ver de las semanas pasadas de Dasney: %.1f\n", StseriesD);
                    printf("Series acumuladas faltantes por ver de las semanas pasadas de Betflix: %.1f\n", StseriesB);
                    printf("-------------------------------------------------------\n");
                }
                auxD = auxD + StseriesD  ; 
                auxB = auxB + StseriesB ;

                printf("Series acumuladas por ver Dasney: %.1f\n", auxD);
                printf("Series acumuladas por ver Netflix: %.1f\n", auxB);

            }

            printf("-------------------------------------------------------\n");
            for(int i = 0; i < Pd; i++){
                printf("El profesor %d vio %.1f series de Dasney\n", i + 1, TdeCadaProfesorD[i]);
            }
            printf("-------------------------------------------------------\n");
            for (int i = 0; i < Pb; i++){
                printf("El profesor %d vio %.1f series de Betflix\n", i + 1, TdeCadaProfesorB[i]);
            }
            printf("-------------------------------------------------------\n");
            for(int i = 0; i < Pd; i++){
                sumatoriaD += TdeCadaProfesorD[i];
            }
            for(int i = 0; i < Pb; i++){
                sumatoriaB += TdeCadaProfesorB[i];
            }
            printf("Los profesores vieron %.1f series de Dasney\n", sumatoriaD);
            printf("Los profesores vieron %.1f series de Betflix\n", sumatoriaB);
            printf("-------------------------------------------------------\n");
            StseriesD += TseriesD;
            StseriesB += TseriesB;

            
            printf("Quedan %.1f series de totales de Dasney por ver\n", StseriesD);
            printf("Quedan %.1f series de totales de Betflix por ver\n", StseriesB);


        }
        
    }

    free(arg);
    return NULL;
}


int main(void) {
    srand((unsigned int)time(NULL));
    pthread_t PD[Pd];  // Profesores que ven Dasney
    pthread_t PB[Pb];  // Profesores que ven Betflix

    sem_init(&semaforoD, 0, Pd);
    sem_init(&semaforoB, 0, Pb);

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

    printf("Tiempo: %d\n", Ctiempo);

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