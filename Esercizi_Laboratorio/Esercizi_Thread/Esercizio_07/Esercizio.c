/*Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria
Pthread, lancia m thread per calcolare il prodotto di due matrici di dimensione mxn e nxp.
Non appena sarà calcolata la matrice prodotto, un m+1-esimo thread, che era in attesa,
provvederà a stampare la matrice risultato. Le matrici devono essere allocate dinamicamente. 
Usare i semafori Posix basati su nome.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int **A, **B, **Prodotto;
int m=0, n=0, p=0;

sem_t *sem;

void* moltiplicamatrice(void* arg)
{
    int indice = *(int*)arg;
    free(arg);

    for(int j=0;j<p;j++){ //colonne della matrice Prodotto
        for(int k=0;k<n;k++) //dimensione comune: colonne di A = righe di B
        Prodotto[indice][j] += A[indice][k] * B[k][j];
    }
    sem_post(sem);

    return NULL;


}

void* stampamatrice(void* arg)
{
    for(int i=0;i<m;i++){
        sem_wait(sem);
    }

    printf("\nMatrice Prodotto: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", Prodotto[i][j]);
        }
        printf("\n");
    }

    return NULL;



}


int main(int argc, char *argv[])
{
    if(argc!=4){
        fprintf(stderr, "USO: %s <integer(m)> <integer(n)> <integer(p)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m=atoi(argv[1]);
    n=atoi(argv[2]);
    p=atoi(argv[3]);

    if(m<=0 || n<=0 || p<=0){
        fprintf(stderr,"m, n e p devono essere interi positivi e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alle matrici
    
    //Matrice A mxn
    A = malloc(m*sizeof(int*));
    if(A == NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
        A[i]= malloc(n*sizeof(int));
        if(A[i] == NULL){
            perror("malloc righe A");
            exit(EXIT_FAILURE);
        }
    }
    
    //Matrice B nxp
    B = malloc(n*sizeof(int*));
    if(B == NULL){
        perror("malloc B");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<n;i++){
        B[i]= malloc(p*sizeof(int));
        if(B[i] == NULL){
            perror("malloc righe B");
            exit(EXIT_FAILURE);
        }
    }

    //Matrice Prodotto mxp
    Prodotto = calloc(m,sizeof(int*));
    if(Prodotto == NULL){
        perror("calloc Prodotto");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
        Prodotto[i]= calloc(p,sizeof(int));
        if(Prodotto[i] == NULL){
            perror("calloc righe");
        }
    }

    //riempio la matrice A con numeri interi pseudocasuali in un range 5 e 30
    srand(time(NULL));

    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            A[i][j] = rand()%(30-5+1)+5;
        }
    }

    //riempio la matrice B con numeri interi pseudocasuali in un range 10 e 25

    for(int i=0;i<n;i++){
        for(int j=0;j<p;j++){
            B[i][j] = rand()%(25-10+1)+10;
        }
    }

    //Stampo la matrici A di partenza
    printf("\nMatrice A di partenza:\n ");
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

    //Stampo la matrice B di partenza
    printf("\nMatrice B di partenza:\n ");
    for(int i=0;i<n;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    //Stampa di debug matrice Prodotto
    printf("\nMatrice Prodotto di partenza: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", Prodotto[i][j]);
        }
        printf("\n");
    }

    // creo il semaforo
    sem_unlink("/miosem123");  // Rimuove eventuale vecchio semaforo con lo stesso nome
    sem = sem_open("/miosem123", O_CREAT | O_EXCL, 0644, 0);
    if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
    }

    //creo m thread
    pthread_t thread[m];
    pthread_t stampatore;

    for(int i=0;i<m;i++){
        int *arg=malloc(sizeof(int));
        if(arg == NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        if(pthread_create(&thread[i], NULL, moltiplicamatrice, arg)!=0){
            perror("pthread_create thread");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_create(&stampatore, NULL, stampamatrice, NULL)!=0){
        perror("pthread_create stampatore");
        exit(EXIT_FAILURE);
    }


    //join dei thread
    for(int i=0;i<m;i++){
        pthread_join(thread[i],NULL);
    }
    pthread_join(stampatore,NULL);

    //dealloco memoria
    
    //Matrice A
    for(int i=0;i<m;i++){
        free(A[i]);
    }
    free(A);

    //Matrice B
    for(int i=0;i<n;i++){
        free(B[i]);
    }
    free(B);

    //Matrice Prodotto
    for(int i=0;i<m;i++){
        free(Prodotto[i]);
    }
    free(Prodotto);

    // Chiudo semaforo
    sem_close(sem);
    sem_unlink("/miosem123");


    return 0;


}