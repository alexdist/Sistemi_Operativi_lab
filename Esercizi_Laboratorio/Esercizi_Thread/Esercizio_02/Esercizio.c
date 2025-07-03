/*Scrivere un programma C che crea M thread produttori e N thread consumatori
(con M = 2*N), con M ed N presi come parametro da riga di comando. Tutti i thread
condividono una variabile intera. Ogni thread produttore incrementa di due unità
la variabile condivisa se e solo se il valore della stessa è inferiore a 100.
Ogni thread consumatore decrementa di due unità la variabile condivisa se e solo se il
valore della stessa è superiore a 25. Tutti i thread, dopo l'operazione sulla variabile
condivisa (di incremento o di decremento) dormono per 2 secondi. Per implementare la 
sincronizzazione utilizzare i semafori Posix basati su nome*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

int N=0;
int M=0;

sem_t *sembin;

typedef struct{

    int x;

}Glob;


void* produttore(void* arg)
{
    Glob* glob = (Glob*)arg;
while(1){
    sem_wait(sembin);
    
    if (glob->x < 100) {
    glob->x += 2;
    printf("Produttore. X: %d\n", glob->x);
}   
    else {
    printf("Produttore. X rimasto a: %d (limite raggiunto)\n", glob->x);
}
    sem_post(sembin);


    sleep(2);
    }

    return NULL;

}

void* consumatore(void* arg)
{
    Glob* glob = (Glob*)arg;

    while(1){

  sem_wait(sembin);
    if (glob->x > 25) {
    glob->x -= 2;
    printf("Consumatore. X: %d\n", glob->x);
    } 
        else {
    printf("Consumatore ha trovato X = %d e non ha fatto nulla (troppo basso)\n", glob->x);
    }
    sem_post(sembin);

    sleep(2);
    }

    return NULL;

}

int main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO: %s <integer(N)>\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);

    if(N<=0){
        fprintf(stderr, "N deve essere positivo e >0!\n");
        exit(EXIT_FAILURE);
    }

    M = 2*N;


    //inizializzo semaforo con nome
    sembin = sem_open("/miosembin", O_CREAT, 0644, 1);


    //inizializzo struttura dati Glob 
    Glob *glob = malloc(sizeof(Glob));
    glob->x = 0;


    //creo i thread produttori

    pthread_t produttori[M];

    for(int i=0;i<M;i++){

        pthread_create(&produttori[i], NULL, produttore, (void*)glob);
    }

    pthread_t consumatori[N];

    for(int i=0;i<N;i++){

        pthread_create(&consumatori[i], NULL, consumatore, (void*)glob);
    }

    //join dei thread produttori
    for(int i=0; i<M;i++){
        pthread_join(produttori[i], NULL);
    }

    for(int i=0;i<N;i++){
        pthread_join(consumatori[i], NULL);
    }

    //dealloco memoria alla struttura
    free(glob);
    
    //chiudo semaforo
    sem_close(sembin);
    sem_unlink("/miosembin");

    return 0;



}