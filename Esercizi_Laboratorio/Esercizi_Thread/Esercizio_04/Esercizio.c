/* Si consideri una matrice di caratteri con dimensioni mxn, dove m ed n sono passati
tramite riga di comando. La matrice è allocata dinamicamente. Scrivere un programma in C
(POSIX, ambiente LINUX) che, dato un carattere x come argomento, avvii n thread che esplorino
ciascuno una colonna alla ricerca del carattere x. Ogni thread, trovata un'occorrenza, aggiorna
un contatore in un array chiamato trovati di dimensione n. Un thread aggiuntivo deve attendere
la fine dell'elaborazione per poi stampare l'intero array trovati. Usare variabili di condizione. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

char **A;
int *trovati;
int threadterminati = 0;
int m=0, n=0;
char x;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* trovaoccorrenza(void* arg){

    int col = *(int*)arg;
    free(arg);

    int occorrenza = 0;

    for(int i=0;i<m;i++){
        if(A[i][col] == x)
            occorrenza++;
    }

    pthread_mutex_lock(&mutex);

    trovati[col] = occorrenza;

    threadterminati++;
    if(threadterminati == n){ //se è l'ultimo thread segnala lo stampatore
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* stampaarray(void* arg){

    pthread_mutex_lock(&mutex);
    while(threadterminati < n){
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Array trovati:\n ");
    for(int i=0;i<n;i++){
        printf("%2d ", trovati[i]);
    }
    printf("\n");

    return NULL;

}

int main(int argc, char* argv[])
{
    if(argc!=4){
        fprintf(stderr, "USO: %s <integer(m)> <integer(n)> <char(x)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    x = argv[3][0];

    //alloco memoria alla matrice di caratteri
    
    A = malloc(m*sizeof(char*));
    if(A == NULL){
        perror("malloc matrice");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++){
       A[i]=malloc(n*sizeof(char));
       if(A[i]==NULL){
        perror("malloc riga");
        exit(EXIT_FAILURE);
       }
    }

    //alloco memoria al vettore "trovati"
    
    trovati = calloc(n,sizeof(int));
    if(trovati == NULL){
        perror("calloc trovati");
        exit(EXIT_FAILURE);
    }
    

    //riempio la matrice con caratteri pseudocasuali
    srand(time(NULL));

    for(int i=0; i<m;i++){
        for(int j=0;j<n;j++){
            A[i][j]= 'a'+rand()%26;
        }
    }

    //stampo la matrice di caratteri
    printf("\n Matrice: \n");
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            printf("%4c ", A[i][j]);
        }
        printf("\n");
    }

    //stampo l'array trovati (debug)
    /*printf("\n Array trovati:\n");
    for(int i=0;i<n;i++){
        printf("%2d ", trovati[i]);
    }
    printf("\n");*/

    //creo i thread lavoratori e il thread stampatore
    pthread_t thread[n];
    pthread_t stampatore;

    for(int i=0;i<n;i++){
        int *arg = malloc(sizeof(int));
        if(arg == NULL){
            perror("malloc arg");
            exit(EXIT_FAILURE);
        }
        *arg = i;

        pthread_create(&thread[i], NULL, trovaoccorrenza, arg);
    }
    pthread_create(&stampatore, NULL, stampaarray, NULL);


    //join dei thread
    for(int i=0;i<n;i++){
        pthread_join(thread[i], NULL);
    }
    pthread_join(stampatore, NULL);


    //distruggo mutex e variabile di condizione
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);


    //dealloco memoria alla matrice
    for(int i=0;i<m;i++){
        free(A[i]);
    }
    free(A);
    free(trovati);
    
    return 0;
}   
