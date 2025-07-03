/*Scrivere un programma C, secondo lo standard Posix, in ambiente Linux, che date due matrici
A e B di dimensioni nxm di interi tra 0 e 255, ne determini una matrice C in cui ogni entrata
C(i,j) è ottenuta dalla divisione A(i,j)/B(i,j). Il calcolo avviene concorrentemente usando
m threads, uno per colonna. Appena un thread termina il calcolo della propria colonna di C, attende
che tutti gli altri thread abbiano finito. A quel punto, un thread n+1-esimo stamperà la matrice C.
Usare i semafori Posix.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int **A, **B, **C;
int n=0, m=0;
int count = 0;


sem_t barriera;
sem_t via_stampatore;
sem_t semaforobinario;

void* dividimatrice(void* arg)
{
    int col = *(int*)arg;
    free(arg);

    for(int i=0;i<n;i++){
        
        if(B[i][col]!=0)
        C[i][col] = A[i][col]/B[i][col];
        
        else
            C[i][col] = 0;
    }
    
    //Fase di barriera manuale
    
    sem_wait(&semaforobinario); //sezione critita
    count++;
    if(count == m){
        //ultimo thread: sblocca tutti i worker
        for(int i=0;i<m;i++){
            sem_post(&barriera);
        }
        //sblocca anche lo stampatore
        sem_post(&via_stampatore);
    }
    sem_post(&semaforobinario);
    


    //tutti aspettano qui fino a che non sono stati tutti contati
    sem_wait(&barriera);

    return NULL;

}

void* stampamatrice(void* arg)
{
    sem_wait(&via_stampatore);

    printf("\nMatrice C risultato: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", C[i][j]);
        }
        printf("\n");
    }
    return NULL;

}

int main(int argc, char *argv[])
{
    if(argc!=3){
        fprintf(stderr, "USO: %s <integer(n)> <integer(m)>\n");
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);

    if(n<=0 || m<=0)
    {
        fprintf(stderr, "n e m devono essere positivi e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alle matrici
    A = malloc(n*sizeof(int*));
    if(A == NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }

    B = malloc(n*sizeof(int*));
    if(B==NULL){
        perror("malloc B");
        exit(EXIT_FAILURE);
    }

    C = calloc(n,sizeof(int*));
    if(C==NULL){
        perror("calloc C");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<n;i++){
        A[i]=malloc(m*sizeof(int));
        B[i]=malloc(m*sizeof(int));
        C[i]=calloc(m,sizeof(int));
    }


    //riempio le matrici con numeri pseudocasuali
    srand(time(NULL));

    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            A[i][j]=rand()%256;
            B[i][j]=rand()%256;
        }
    }


    //stampo le matrici di partenza
    printf("\nMatrice A: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

    printf("Matrice B: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    printf("Matrice C di partenza: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", C[i][j]);
        }
        printf("\n");
    }

    //inizializzo thread e semaforo
    sem_init(&barriera, 0, 0);
    sem_init(&via_stampatore,0,0);
    sem_init(&semaforobinario, 0, 1);

    pthread_t threads[m];
    pthread_t stampatore;

    for(int i=0;i<m;i++){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, dividimatrice, arg);
    }
    pthread_create(&stampatore, NULL, stampamatrice, NULL);


    //join dei thread
    for(int i=0;i<m;i++){
        pthread_join(threads[i], NULL);
    }
    pthread_join(stampatore, NULL);

    //distruggo semaforo
    sem_destroy(&barriera);
    sem_destroy(&via_stampatore);
    sem_destroy(&semaforobinario);
   
    

    //dealloco memoria alle matrici
    for(int i=0;i<n;i++){
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return 0;
    
}