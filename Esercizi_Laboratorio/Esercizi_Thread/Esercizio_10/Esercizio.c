/*Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria
Pthread, lancia m thread per calcolare la somma dei prodotti delle colonne di due matrici mxm.
L'i-esimo thread, dopo aver calcolato la somma dei prodotti delle colonne i-esime delle due matrici,
inserisce il risultato in un array m-dimensionale, in modo concorrente, nella prima locazione libera disponibile.
Non appena l'array sarà riempito, un m+1-esimo thread, che era in attesa, provvederà a stamparne il contenuto.
Le matrici devono essere allocate dinamicamente. Usare mutex e variabili di condizione.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **A, **B;
int *v;
int m=0;
int indicearray=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* sommaprodotti(void *arg)
{   
    int col = *(int*)arg;
    free(arg);
    int sommaprodotti=0;

    for(int i=0;i<m;i++){
        sommaprodotti += A[i][col]*B[i][col];
    }

    pthread_mutex_lock(&mutex);
    v[indicearray] = sommaprodotti;
    indicearray++;
    
    if(indicearray == m){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

    printf("Thread colonna %d: somma = %d\n", col, sommaprodotti);


    return NULL;


}

void* stampavettore(void* arg)
{
    pthread_mutex_lock(&mutex);
    while(indicearray < m){
        pthread_cond_wait(&cond, &mutex);
    }

    printf("\nContenuto Array: \n");
    for(int i=0;i<m;i++){
        printf("%2d ", v[i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);

    return NULL;

}

int main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO: %s <integer(m)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m=atoi(argv[1]);
    if(m<=0){
        fprintf(stderr, "m deve essere un numero intero positivo e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alla matrice A
    A=malloc(m*sizeof(int*));
    if(A==NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
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
        B[i]=malloc(m*sizeof(int));
        if(B[i]==NULL){
            perror("malloc righe B");
            exit(EXIT_FAILURE);
        }
    }

    //alloco memoria al vettore v
    v = calloc(m,sizeof(int));
    if(v==NULL){
        perror("calloc");
        exit(EXIT_FAILURE);
    }


    //riempio la matrice A con numeri interi pseudocasuali nel range 2-8
    srand(time(NULL));

    for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
            A[i][j]= rand()%(8-2+1)+2;
        }
    }


    //riempio la matrice B con numeri interi pseudocasuali nel range 3-9

    for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
            B[i][j]= rand()%(9-3+1)+3;
        }
    }


    //Stampo la matrice A di partenza
    printf("\nMatrice A: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }


    //Stampo la matrice B di partenza
    printf("Matrice B: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }


    /*//stampa di debug vettore
    printf("Vettore v: \n");
    for(int i=0;i<m;i++){
        printf("%2d ", v[i]);
    }*/


    //creo i thread

    pthread_t thread[m];
    pthread_t stampatore;

    if(pthread_create(&stampatore, NULL, stampavettore, NULL)!=0){
        perror("pthread_create stampatore");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<m;i++){
        int *arg = malloc(sizeof(int));
        if(arg == NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;

        if(pthread_create(&thread[i], NULL, sommaprodotti, arg)!=0){
            perror("pthread_create thread");
            exit(EXIT_FAILURE);
        }
    }


    //join dei thread

    for(int i=0;i<m;i++){
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
    if(pthread_mutex_destroy(&mutex)!=0){
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_destroy(&cond)!=0){
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);
    }

    
    //dealloco memoria
    for(int i=0;i<m;i++){
        free(A[i]);
    }
    free(A);

    for(int i=0;i<m;i++){
        free(B[i]);
    }
    free(B);

    free(v);

    return 0;
}