/*Scrivere un programma in C e Posix sotto Linux che utilizzi la 
libreria Pthread per risolvere il seguente problema: date 
due matrici A e B, di dimensioni nxn di numeri interi tra 
1 e 100 generati casualmente, creare n thread dove l'i-esimo 
thread provvede a sommare le entrate delle righe i-esime 
delle due matrici in modo da ottenere una matrice Somma 
di dimensioni nxn. Sincronizzare i thread in modo da sommare 
le righe delle due matrici A e B nell'ordine 1, 2, ..., n. 
Inoltre, un thread n+1-esimo attende che la matrice Somma sia 
calcolata, per stamparne, successivamente, il contenuto. 
Usare semafori e variabili di condizione. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


int **A, **B, **Somma;
int n=0;
int indicecorrente = 0;
int threadfinito = 0;


sem_t sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void* sommaentrate(void* arg)
{
    int indice = *(int*)arg;
    free(arg);

    pthread_mutex_lock(&mutex);
    while(indice != indicecorrente){
        pthread_cond_wait(&cond, &mutex);
    }
    
    //calcola la riga della somma
    for(int j=0;j<n;j++){
    
        Somma[indice][j] = A[indice][j] + B[indice][j];
    }

    indicecorrente++;
    threadfinito++;

    if(threadfinito == n){ //l'ultimo worker thread risveglia il thread stampatore
    sem_post(&sem);
    }

    pthread_cond_signal(&cond); 
    
    pthread_mutex_unlock(&mutex);

     return NULL;

}

void* stampamatrice(void* arg)
{   
    sem_wait(&sem);

    printf("\nMatrice SOMMA finale: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", Somma[i][j]);
        }
        printf("\n");
    }
    return NULL;

}

int main(int argc, char* argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO: %s <integer(n)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    if(n<=0){
        fprintf(stderr, "ERRORE: n deve essere un numero positivo >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alle matrici
    A = malloc(n*sizeof(int*));
    B = malloc(n*sizeof(int*));
    Somma = calloc(n,sizeof(int*));

    if(A == NULL || B == NULL || Somma == NULL){
        perror("malloc matrici");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<n;i++){
        A[i]=malloc(n*sizeof(int));
        B[i]=malloc(n*sizeof(int));
        Somma[i]=calloc(n,sizeof(int));

        if(A[i]==NULL || B[i]==NULL || Somma[i]==NULL){
            perror("malloc righe");
            exit(EXIT_FAILURE);
        }
    }

    //riempio le matrici A e B con numeri interi pseudocasuali da 1 a 100

    srand(time(NULL));

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            A[i][j]= rand()%100+1;
            B[i][j]= rand()%100+1;
        }
    }

    //stampa iniziale delle matrici A, B e Somma

    printf("\nMatrice A: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }


    printf("Matrice B: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    printf("Matrice Somma di partenza: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", Somma[i][j]);
        }
        printf("\n");
    }


    //inizializzazione semaforo

    sem_init(&sem, 0, 0);
    
    //inizializzazione thread
    pthread_t thread[n];
    pthread_t stampatore;

    //creo thread lavoratori
   for(int i=0;i<n;i++)
   {
    int *arg = malloc(sizeof(int));
    if(arg == NULL){
        perror("malloc arg");
        exit(EXIT_FAILURE);
    }
    *arg = i;

    if(pthread_create(&thread[i], NULL, sommaentrate, arg) != 0){
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
   }
    if(pthread_create(&stampatore, NULL, stampamatrice, NULL)!=0){
        perror("pthread_create stampatore");
        exit(EXIT_FAILURE);
    }


    //join dei thread

    for(int i=0;i<n;i++){
        if(pthread_join(thread[i], NULL)!=0){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_join(stampatore, NULL)!=0){
        perror("pthread_join stampatore");
        exit(EXIT_FAILURE);
    }

    //distruggo mutex, variabile di condizione e semaforo

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);


    //dealloco memoria alle matrici
    for(int i=0;i<n;i++){
        free(A[i]);
        free(B[i]);
        free(Somma[i]);
    }
    free(A);
    free(B);
    free(Somma);

    return 0;

}