/*Scrivere un programma in C che (in ambiente Linux e utilizzando la libreria Pthread)
crei 2 thread che eseguono la funzione "incrementa" che a sua volta accede alle variabili
glob.a e glob.b di una struttura dati condivisa glob e ne incrementi il loro valore di
di 1 per 100 volte. Al termine, quando i thread avranno terminato con gli incrementi, il
thread principale stamperà a video i valori delle variabili test.a e test.b. Per la
gestione della sincronizzazione si utilizzino i mutex allocati dinamicamente.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct{

    int a;
    int b;

    pthread_mutex_t *mutex; //Puntatore a mutex(allocato dinamicamente)

}Glob;

void* incrementa(void* arg)
{
    Glob *glob = (Glob*)arg; //Cast del parametro generico a puntatore alla struttura Glob

    for(int i=0;i<100;i++){

        pthread_mutex_lock(glob->mutex); //Inizio sezione critica
        glob->a +=1;
        glob->b +=1;
        pthread_mutex_unlock(glob->mutex); //Fine sezione critica

    }
    return NULL;


}


int main(){

    Glob *glob = malloc(sizeof(Glob)); //Allocazione dinamica della struttura Glob
    if(glob == NULL){
        perror("malloc glob");
        exit(EXIT_FAILURE);
    }
    //inizializza a e b
    glob->a = 0;
    glob->b = 0;
    
    glob->mutex = malloc(sizeof(pthread_mutex_t)); //Allocazione dinamica del mutex
    if(glob->mutex == NULL){
        perror("malloc mutex");
        exit(EXIT_FAILURE);
    }

    if(pthread_mutex_init(glob->mutex, NULL)!=0){ //Inizializzazione del mutex
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    //Creazione dei due thread
    pthread_t thread1;
    pthread_t thread2;

    if(pthread_create(&thread1, NULL, incrementa, glob)!=0){
        perror("pthread_create thread1");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&thread2, NULL, incrementa, glob)!=0){
        perror("pthread_create thread2");
        exit(EXIT_FAILURE);
    }

    //Attesa del termine dei thread
    if(pthread_join(thread1, NULL)!=0){
        perror("pthread_join thread1");
        exit(EXIT_FAILURE);
    }

    if(pthread_join(thread2, NULL)!=0){
        perror("pthread_join thread2");
        exit(EXIT_FAILURE);
    }
 
    printf("Valore di glob.a = %d\n", glob->a);
    printf("Valore di glob.b = %d\n", glob->b);

    //Distruzione del mutex e deallocazione della memoria
    pthread_mutex_destroy(glob->mutex);
    free(glob->mutex);
    free(glob);
    

    return 0;
}