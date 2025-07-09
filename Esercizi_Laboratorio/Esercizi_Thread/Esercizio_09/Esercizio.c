/*Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria
Pthread, lancia k thread per calcolare il prodotto di due matrici di dimensione kxm e mxp.
Non appena sarà calcolata la matrice prodotto, un k+1-esimo thread aggiuntivo, che era rimasto
in attesa, provvederà a stampare la matrice risultato. Le matrici devono essere allocate dinamicamente.
Usare le variabili di condizione.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **A, **B, **Prodotto;
int k=0, m=0, p=0;
int finishthread=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* calcolaprodotto(void* arg)
{
    int indice = *(int*)arg;
    free(arg);

    for(int j=0;j<p;j++){ //colonne della matrice Prodotto
        for(int x=0;x<m;x++){ //dimensione comune: colonne di A = righe di B
            Prodotto[indice][j]+= A[indice][x] * B[x][j];
        }
    }

    pthread_mutex_lock(&mutex);
    finishthread++;
    if(finishthread == k){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;

}

void* stampamatrice(void* arg)
{
    pthread_mutex_lock(&mutex);
    while(finishthread < k){
        pthread_cond_wait(&cond, &mutex);
    }
    printf("\nMatrice Prodotto:\n ");
    for(int i=0;i<k;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", Prodotto[i][j]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc!=4){
        fprintf(stderr,"USO: %s <integer(k)> <integer(m)> <integer(p)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    k = atoi(argv[1]);
    m = atoi(argv[2]);
    p = atoi(argv[3]);

    if(k<=0 || m<=0 || p<=0){
        fprintf(stderr, "k, m, p devono essere positivi e >0!\n");
        exit(EXIT_FAILURE);
    }

    //alloco memoria alle matrice A
    A = malloc(k*sizeof(int*));
    if(A==NULL){
        perror("malloc A");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<k;i++){
        A[i]= malloc(m*sizeof(int));
        if(A[i]==NULL){
            perror("malloc righe A");
            exit(EXIT_FAILURE);
        }
    }

    //alloco memoria alla matrice B
    B = malloc(m*sizeof(int*));
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

    //alloco memoria alla matrice Risultato
    Prodotto = calloc(k, sizeof(int*));
    if(Prodotto == NULL){
        perror("calloc Prodotto");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<k;i++){
        Prodotto[i]=calloc(p, sizeof(int));
        if(Prodotto[i]==NULL){
            perror("calloc righe Prodotto");
            exit(EXIT_FAILURE);
        }
    }


    //riempio la matrice A e B con numeri interi pseudocasuali
    srand(time(NULL));

    //Matrice A (kxm): numeri interi pseudocasuali nel range 3-12
    for(int i=0;i<k;i++){
        for(int j=0;j<m;j++){
            A[i][j]=rand()%(12-3+1)+3;
        }
    }

    //Matrice B (mxp): numeri interi pseudocasuali nel range 2-10
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            B[i][j]= rand()%(10-2+1)+2;
        }
    }


    //Stampa matrici A e B di partenza

    printf("\nMatrice A: \n");
    for(int i=0;i<k;i++){
        for(int j=0;j<m;j++){
            printf("%4d ",A[i][j]);
        }
        printf("\n");
    }

    printf("Matrice B: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    /*//stampa di debug matrice prodotto iniziale
    printf("\nMatrice Prodotto iniz: \n");
    for(int i=0;i<k;i++){
        for(int j=0;j<p;j++){
            printf("%4d ", Prodotto[i][j]);
        }
        printf("\n");
    }*/

    //creo i thread
    pthread_t thread[k];
    pthread_t stampatore;

    //creo prima il thread stampatore in modo tale che parte prima dei thread lavoratori, e si blocca subito sulla variabile di condizione.
    //In questo modo sono sicuro che l'ultimo thread lavoratore farà pthread_cond_signal quando lo stampatore sarà già sicuramente bloccato
    //in attesa di ricevere il segnale, evitando così un possibile deadlock.
    if(pthread_create(&stampatore, NULL, stampamatrice, NULL)!=0){
        perror("pthread_create stampatore");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<k;i++){
        int *arg = malloc(sizeof(int));
        if(arg==NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;

        if(pthread_create(&thread[i], NULL, calcolaprodotto, arg)!=0){
            perror("pthread_create thread");
            exit(EXIT_FAILURE);
        }
    }
    

    //join dei thread
    for(int i=0;i<k;i++){
        if(pthread_join(thread[i], NULL)!=0){
            perror("pthread_join thread");
        }
    }
    if(pthread_join(stampatore,NULL)!=0){
        perror("pthread_join stampatore");
        exit(EXIT_FAILURE);
    }


    //dealloco memoria alle matrici
    for(int i=0;i<k;i++){
        free(A[i]);
    }
    free(A);

    for(int i=0;i<m;i++){
        free(B[i]);
    }
    free(B);

    for(int i=0;i<k;i++){
        free(Prodotto[i]);
    }
    free(Prodotto);


    return 0;
}