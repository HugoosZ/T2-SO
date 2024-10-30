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
int P = 0;
sem_t semaforoD;
sem_t semaforoB;

pthread_mutex_t mutexD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexbool = PTHREAD_MUTEX_INITIALIZER;

// Función que retorna un valor aleatorio del arreglo t[]
float t_serie(void){
    int indice = rand() % 4;
    return t[indice];
}
int c_series(void){
    int series = rand() % 6 + 10;
    return series;
}

void *verserie(void *arg){
    int id = *(int *)arg;  //B>7
    float Tvisualizacion = t_serie();
    sem_wait(&semaforoD);
    sem_wait(&semaforoB);
    if(id < (P + 1)){
        pthread_mutex_lock(&mutexD);
        sleep(1);
        pthread_mutex_lock(&mutexB);
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexD);
    } else {
        pthread_mutex_lock(&mutexB);
        sleep(1);
        pthread_mutex_lock(&mutexD);
        pthread_mutex_unlock(&mutexD);
        pthread_mutex_unlock(&mutexB);
    }
    if(id < (P + 1)){

        if(TseriesD >= Tvisualizacion){
            TseriesD = TseriesD - Tvisualizacion;
            TdeCadaProfesorD[id - 1] =  Tvisualizacion;
        }
        else{
            Tvisualizacion = TseriesD;
            TdeCadaProfesorD[id - 1] =  Tvisualizacion;
        }
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexD);

    }else {

        if (TseriesB > Tvisualizacion){
            TseriesB = TseriesB - Tvisualizacion;
            TdeCadaProfesorB[id - 7] =  Tvisualizacion;
        }
        else{
            Tvisualizacion = TseriesB;
            TdeCadaProfesorB[id - 7] =  Tvisualizacion;
        }
        pthread_mutex_unlock(&mutexD);
        pthread_mutex_unlock(&mutexB);

    }   

    printf("Profesor %d terminó de ver la serie\n", id);
    free(arg); 
    return NULL;
}

int main(void){

    srand((unsigned int)time(NULL));    
    int Pd = 6;
    P = Pd;
    int Pb = 6;
    pthread_t PD[Pd];  // Profesores que ven Dasney
    pthread_t PB[Pb];  // Profesores que ven Betflix

    sem_init(&semaforoD, 0, Pd);
    sem_init(&semaforoB, 0, Pb);
    int N = 0;
    int Ctiempo = 0;
    printf("DEADLOCKKKK\n");

    printf("Ingrese el tiempo:\n");
    printf("[1] 1 Mes\n");
    printf("[2] 6 Mes\n");
    printf("[3] 1 Año\n");

    scanf("%d", &N);
    if(N <= 0 || N > 3){
        while(1){
            printf("El número debe ser mayor a 0 y menor a 3\n");
            printf("Ingrese el tiempo:\n");
            scanf("%d", &N);
            if(N > 0 && N < 4){
                break;
            }
        }
    }
    if(N == 1){
        Ctiempo = 4;
    }else if(N == 2){
        Ctiempo = 24;
    }else if(N == 3){
        Ctiempo = 48;
    }

    printf("Tiempo: %d\n", Ctiempo);
    pthread_mutex_init(&mutexD, NULL);
    pthread_mutex_init(&mutexB, NULL);
    pthread_mutex_init(&mutexbool, NULL);

    while(Ctiempo > 0){ 
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

        for(int i = 0; i < Pd; i++){
            int *idD = malloc(sizeof(int));  // Reservar memoria para cada id
            *idD = i + 1; 
            pthread_create(&PD[i], NULL, verserie, (void *)idD);
        }
        for(int i = 0; i < Pb; i++){
            int *idB = malloc(sizeof(int));  
            *idB = i + Pd + 1;
            pthread_create(&PB[i], NULL, verserie, (void *)idB);
        }


        for(int i = 0; i < 6; i++){
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
