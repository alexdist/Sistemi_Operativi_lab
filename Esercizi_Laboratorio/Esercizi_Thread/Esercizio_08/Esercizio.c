/* Scrivere un programma C, secondo lo standard Posix, in ambiente Linux, che realizzi il
prodotto di due matrici di dimensioni nxm e mxp, in maniera concorrente, utilizzando n thread.
Un thread n+1-esimo attenderà il calcolo della matrice prodotto per poi stamparne il contenuto.*/

//La traccia non richiede esplicitamente un meccanismo di sincronizzazione. Userò i mutex e variabili di cond.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **A, **B, **Prodotto;
int n=0, m=0, p=0;
int finishthread=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* calcolaprodotto(void* arg)
{
    int indice = *(int*)arg;
    free(arg);

    for(int j=0;j<p;j++){ //colonne della matrice Prodotto
        for(int k=0;k<m;k++){ //dimensione comune colonne di A = righe di B
            
            Prodotto[indice][j]+= A[indice][k] * B[k][j];
        }
    }
    pthread_mutex_lock(&mutex);
    finishthread++;
    if(finishthread == n){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;

}

void* stampamatrice(void* arg)
{
    pthread_mutex_lock(&mutex);
    while(finishthread < n){
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("\nMatrice Prodotto: \n");

    for(int i=0;i<n;i++){
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
        fprintf(stderr, "USO: %s <integer(n)> <integer(m)> <integer(p)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);
    p = atoi(argv[3]);

    if(n<=0 || m<=0 || p<=0){
        fprintf(stderr, "n, m e p devono essere numeri interi positivi e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alle matrici

    A = malloc(n*sizeof(int*));
    if(A == NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<n;i++){
        A[i]=malloc(m*sizeof(int));
        if(A[i]==NULL){
            perror("malloc righe A");
            exit(EXIT_FAILURE);
        }
    }

    B = malloc(m*sizeof(int*));
    if(B == NULL){
        perror("malloc B");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
        B[i] = malloc(p*sizeof(int));
        if(B[i] == NULL){
            perror("malloc righe B");
            exit(EXIT_FAILURE);
        }
    }

    Prodotto = calloc(n, sizeof(int*));
    if(Prodotto == NULL){
        perror("calloc Prodotto");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<n;i++){
        Prodotto[i] = calloc(p,sizeof(int));
        if(Prodotto[i]==NULL){
            perror("calloc righe Prodotto");
            exit(EXIT_FAILURE);
        }
    }

    //riempio la matrice A (nxm) con numeri interi pseudocasuali nel range 15-30
    srand(time(NULL));

    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            A[i][j] = rand()%(30-15+1)+15;
        }
    }

    //riempio la matrice B (mxp) con numeri interi pseudocasuali nel range 5-20

    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            B[i][j] = rand()%(20-5+1)+5;
        }
    }


    //Stampo le matrici A e B di partenza

    printf("\nMatrice A:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

    printf("\nMatrice B:\n");
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    //creo n thread

    pthread_t thread[n];
    pthread_t stampatore;

    for(int i=0;i<n;i++){
    int *arg = malloc(sizeof(int));
    if(arg == NULL){
        perror("malloc arg");
        exit(EXIT_FAILURE);
    }
    *arg = i;

    if(pthread_create(&thread[i], NULL, calcolaprodotto, arg) != 0){
        perror("pthread_create thread");
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
            perror("pthread_join thread");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_join(stampatore, NULL)!=0){
        perror("pthread_join stampatore");
        exit(EXIT_FAILURE);
    }

    //distruggo mutex e variabile di condizione
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);


    /*//Stampa di debug matrice Prodotto
    printf("\nMatrice Prodotto:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", Prodotto[i][j]);
        }
        printf("\n");
    }*/


    //dealloco memoria alle matrici

    for(int i=0;i<n;i++){
        free(A[i]);
    }
    free(A);

    for(int i=0;i<m;i++){
        free(B[i]);
    }
    free(B);

    for(int i=0;i<n;i++){
        free(Prodotto[i]);
    }
    free(Prodotto);

    return 0;
}