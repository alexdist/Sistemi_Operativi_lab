/*Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria Pthread, lancia
m thread concorrenti per cercare il massimo di ciascuna delle m righe di una matrice mxn di interi
e scriverlo in un array di dimensione m nella prima posizione libera disponibile. Un thread m+1-esimo,
completato il lavoro dei primi m thread, provvede a cercare il massimo dei massimi e a stamparlo. Usare
semafori Posix basati su nome e variabili di condizione. La dimensione della matrice può essere fornita
in input al programma in fase di esecuzione o da riga di comando*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

int **A;
int *v;
int m=0,n=0;
int indicev=0;
int finishthread=0;

//sem_t *sem = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void* cercamaxrighe(void* arg)
{
    int riga = *(int*)arg;
    free(arg);

    int massimo = INT_MIN;

    for(int j=0;j<n;j++){
        if(A[riga][j] > massimo){
            massimo = A[riga][j];
        }
    }
    printf("Thread %d ha trovato massimo %d\n", riga, massimo);

    pthread_mutex_lock(&mutex);
    v[indicev]=massimo;
    indicev++;
    finishthread++;
    if(finishthread == m){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

   // sem_post(sem);

    return NULL;


}

void* stampaecercamax(void* arg)
{   //Attendo con il semaforo m completamenti
   /* for(int i=0;i<m;i++){
        sem_wait(sem);
    }*/

    pthread_mutex_lock(&mutex);
    while(finishthread < m){
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    
    int massimoassoluto=v[0];

    for(int i=1;i<m;i++){
        if(v[i]>massimoassoluto)
        massimoassoluto = v[i];
    }

    printf("\n\nMassimo dei massimi: %d", massimoassoluto);
    printf("\n");
    

}

int main(int argc, char* argv[])
{
    if(argc!=3){
        fprintf(stderr,"USO: %s <integer(m)> <integer(n)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m=atoi(argv[1]);
    n=atoi(argv[2]);

    if(m<=0 || n<=0){
        fprintf(stderr,"m ed n devono essere positivi e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alla matrice A

    A=malloc(m*sizeof(int*));
    if(A==NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
        A[i]=malloc(n*sizeof(int));
        if(A[i]==NULL){
            perror("malloc righe A");
            exit(EXIT_FAILURE);
        }
    }

    //alloco memoria al vettore v
    v=calloc(m,sizeof(int));
    if(v==NULL){
        perror("calloc v");
        exit(EXIT_FAILURE);
    }

    //riempio la matrice A con numeri interi pseudocasuali nel range 

    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            A[i][j]=rand()%30;
        }
    }

    //stampo la matrice A di partenza
    printf("\nMatrice A: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

   /* //stampa di debug vettore v
    printf("\nVettore v: \n");
    for(int i=0;i<m;i++){
        printf("%2d ", v[i]);
    }
    printf("\n");*/


    //creo m thread
    pthread_t thread[m];
    pthread_t cercamassimo;

    for(int i=0;i<m;i++){
        int *arg = malloc(sizeof(int));
        if(arg ==NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        if(pthread_create(&thread[i], NULL, cercamaxrighe, arg)!=0){
            perror("pthread_create thread");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_create(&cercamassimo, NULL, stampaecercamax, NULL)!=0){
        perror("pthread_create cercamassimo");
        exit(EXIT_FAILURE);
    }

    //creo semaforo
   // sem = sem_open("/miosem123", O_CREAT, 0644, 0);
    

    //join dei thread
    for(int i=0;i<m;i++){
        if(pthread_join(thread[i],NULL)!=0){
            perror("pthread_join thread");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_join(cercamassimo, NULL)!=0){
        perror("pthread_join cercamassimo");
        exit(EXIT_FAILURE);
    }

    //dealloco memoria Matrice e vettore
    for(int i=0;i<m;i++){
        free(A[i]);
    }
    free(A);

    free(v);

    //elimino mutex e variabile di condizione
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    //elimino semaforo
    //sem_close(sem);
    //sem_unlink("/miosem123");

    return 0;
}