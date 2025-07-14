/*Scrivere un programma C, secondo lo standard Posix, in ambiente Linux, che realizzi il prodotto di
due matrici di dimensioni nxm e mxp in maniera concorrente, utilizzando n thread. Un thread n+1 esimo
attenderà il calcolo della matrice prodotto per stampare il contenuto. Utilizzare i semafori Posix.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int **A, **B, **Prodotto;
int n=0, m=0, p=0;

sem_t sem;

void *prodottomatrice(void *arg)
{
    int indice = *(int*)arg;
    free(arg);

    for(int j=0;j<p;j++){ //colonne della matrice Prodotto
        for(int k=0;k<m;k++){ //dimensione comune colonne di A = righe di B
            Prodotto[indice][j]+= A[indice][k]*B[k][j];
        }
    }

    printf("Thread %d fa sem_post\n",indice);
    sem_post(&sem);

    return NULL;

}

void *stampamatrice(void *arg)
{
    for(int i=0;i<n;i++){
        sem_wait(&sem);
    }

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
        fprintf(stderr,"USO: %s <integer(n)> <integer(m)> <integer(p)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n=atoi(argv[1]);
    m=atoi(argv[2]);
    p=atoi(argv[3]);

    if(n<=0 || m<=0 || p<=0){
        fprintf(stderr, "n, m e p devono essere numeri interi positivi e > 0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alla matrice A
    A= malloc(n*sizeof(int*));
    if(A==NULL){
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

    //alloco memoria alla matrice B
    B=malloc(m*sizeof(int*));
    if(B==NULL){
        perror("malloc B");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
        B[i]=malloc(p*sizeof(int));
        if(B[i]==NULL){
            perror("malloc righe B");
            exit(EXIT_FAILURE);
        }
    }

    //alloco memoria alla matrice Prodotto
    Prodotto= calloc(n, sizeof(int*));
    if(Prodotto == NULL){
        perror("calloc Prodotto");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<n;i++){
        Prodotto[i]=calloc(p, sizeof(int));
        if(Prodotto[i]==NULL){
            perror("calloc righe Prodotto");
            exit(EXIT_FAILURE);
        }
    }

    //riempio la matrice A con numeri interi pseudocasuali nel range 3-8
    srand(time(NULL));

    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            A[i][j]=rand()%(8-3+1)+3;
        }
    }

    //riempio la matrice B con numeri interi pseudocasuali nel range 2-5

    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            B[i][j]= rand()%(5-2+1)+2;
        }
    }

    //stampo la matrice A
    printf("\nMatrice A: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }

    //stampo la matrice B
    printf("\nMatrice B: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    //inizializzo semafori (DA FARE SEMPRE PRIMA DELLA CREAZIONE DEI THREAD!!)

    if (sem_init(&sem, 0, 0)!=0){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
  

    //creo n thread
    pthread_t thread[n];
    pthread_t stampatore;

    for(int i=0;i<n;i++){

        int *arg = malloc(sizeof(int));
        if(arg==NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;

        if(pthread_create(&thread[i], NULL, prodottomatrice, arg)!=0){
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
        if(pthread_join(thread[i],NULL)!=0){
            perror("pthread_join thread");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_join(stampatore,NULL)!=0){
        perror("pthread_join stampatore");
        exit(EXIT_FAILURE);
    }

    
    //distruggo semafori
    if(sem_destroy(&sem)!=0){
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }
    

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
