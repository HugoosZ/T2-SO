#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>



float TseriesD;
float TseriesB;  
float TD = 0;
float TB = 0;
float t[] = {0.5, 1.0, 1.5, 2.0};
float TdeCadaProfesorD[6];
float TdeCadaProfesorB[6];
float sumatoriaD = 0;
float sumatoriaB = 0;
int Bool = 0;

sem_t semaforo;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

    pthread_mutex_lock(&mutex);    

    if(id < 7){
        
        TseriesD = TseriesD - Tvisualizacion;
        TdeCadaProfesorD[id - 1] =  Tvisualizacion;
    }else {
        TseriesB = TseriesB - Tvisualizacion;
        TdeCadaProfesorB[id - 7] =  Tvisualizacion;

    }   
    Bool++;
    if(Bool == 12){
        printf("-------------------------------------------------------\n");
        for(int i = 0; i < 6; i++){
            printf("El profesor %d vio Dasney por %.1f segundos\n", i + 1, TdeCadaProfesorD[i]);
        }
        printf("-------------------------------------------------------\n");
        for (int i = 0; i < 6; i++){
            printf("El profesor %d vio Betflix por %.1f segundos\n", i + 1, TdeCadaProfesorB[i]);
        }
        printf("-------------------------------------------------------\n");
        for(int i = 0; i < 6; i++){
            sumatoriaD += TdeCadaProfesorD[i];
        }
        for(int i = 0; i < 6; i++){
            sumatoriaB += TdeCadaProfesorB[i];
        }
        printf("Los profesores vieron: %.1f series de Dasney\n", sumatoriaD);
        printf("Los profesores vieron: %.1f series de Betflix\n", sumatoriaB);
        printf("-------------------------------------------------------\n");
        printf("Quedan %.1f series de Dasney\n", TseriesD);
        printf("Quedan %.1f series de Betflix\n", TseriesB);
    }
    // Tiempo aleatorio para ver la serie
    
    pthread_mutex_unlock(&mutex);


    free(arg); 
    return NULL;
}

int main(void){
    sem_init(&semaforo, 0, 1);
    srand(time(NULL)); 
    pthread_t PD[6];  // Profesores que ven Dasney
    pthread_t PB[6];  // Profesores que ven Betflix
    
    TseriesD = c_series();
    TseriesB = c_series();
    int N = 0;
    int Ctiempo = 0;
    printf("Ingrese el tiempo:\n");
    printf("[1] 1 Mes\n");
    printf("[2] 6 Mes\n");
    printf("[3] 1 Año\n");

    scanf("%d", &N);
    if(N < 0 || N > 3){
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

    printf("Series de Dasney: %.1f\n", TseriesD);
    printf("Series de Betflix: %.1f\n", TseriesB);
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < 6; i++){
        int *idD = malloc(sizeof(int));  // Reservar memoria para cada id
        int *idB = malloc(sizeof(int));  
        *idD = i + 1; 
        *idB = i + 7;
        pthread_create(&PD[i], NULL, verserie, (void *)idD);
        pthread_create(&PB[i], NULL, verserie, (void *)idB);
    }

    for(int i = 0; i < 6; i++){
        pthread_join(PD[i], NULL);
        pthread_join(PB[i], NULL);
    }

    return 0;
}
