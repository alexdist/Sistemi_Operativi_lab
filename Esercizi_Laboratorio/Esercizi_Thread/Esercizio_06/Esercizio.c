/*Data una matrice nxn (n pari) di interi generati casualmente, allocata dinamicamente,
con n argomento da riga di comando, creare n thread che prelevano casualmente un elemento
dalla riga di competenza (thread i-esimo, riga i-esima) e lo inseriscano concorrentemente
in un vettore di (n+1)/2 elementi. Un thread n+1-esimo attende il riempimento del vettore
per stampare il contenuto dello stesso e per stampare il numero di elementi inseriti nel vettore
da ciascun thread. Usare mutex e variabili di condizione.*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int **A;
int *v;
int *contatore;
int n=0;
int indicevettore=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void* prelievoelemento(void* arg)
{
    int indice = *(int*)arg;           // Recupera l'indice del thread (passato come puntatore a int)
    free(arg);                         // Libera la memoria allocata per l'argomento

    while(1){                          // Ciclo infinito: il thread continuerà a lavorare finché il vettore non è pieno
    
        pthread_mutex_lock(&mutex);   // Entra nella sezione critica: blocca il mutex per accesso esclusivo alle risorse condivise

        if(indicevettore >= (n+1)/2){ // Se il vettore è già pieno
            pthread_mutex_unlock(&mutex); // Rilascia il mutex
            break;                   // Esce dal ciclo e termina il thread
        }

        int col = rand()%n;          // Sceglie casualmente una colonna nella riga del thread
        int elemento = 0;

        elemento = A[indice][col];   // Preleva l'elemento dalla matrice nella riga del thread e colonna casuale
        v[indicevettore] = elemento; // Inserisce l'elemento nel vettore condiviso
        contatore[indice]++;         // Incrementa il contatore per il thread corrente (quanti elementi ha inserito)
        indicevettore++;             // Avanza l'indice globale del vettore condiviso

        if(indicevettore == (n+1)/2){ // Se il vettore è pieno dopo l'inserimento
            pthread_cond_signal(&cond); // Manda un segnale al thread stampatore (che è in attesa)
        }

        pthread_mutex_unlock(&mutex);   // Rilascia il mutex: altri thread possono entrare nella sezione critica

        usleep(10000);              // Attende 10 millisecondi per evitare starvation e favorire l'interleaving dei thread
    }

    return NULL;                    
}



void* stampavettore(void* arg)
{
    pthread_mutex_lock(&mutex);
    while(indicevettore < (n+1)/2){
        pthread_cond_wait(&cond, &mutex);
    }
    
    printf("\nVettore v:\n");
    for(int i=0;i<(n+1)/2;i++){
        printf("%2d ", v[i]);
    }

    printf("\nNumero elementi inseriti da ciascun thread:\n");
    for(int i=0;i<n;i++){
        printf("Thread %d -> %d\n",i, contatore[i]);
    }
    
    pthread_mutex_unlock(&mutex);

    return NULL;

}


int main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO: %s <integer(n)>", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    if(n<=0 || n%2 != 0){
        fprintf(stderr, "n deve essere un numero pari, positivo e >0!\n");
        exit(EXIT_FAILURE);
    }

    //size del vettore
    int sizev = (n+1)/2;

    //alloco memoria alla matrice e al vettore

    A = malloc(n*sizeof(int*));
    if(A == NULL){
        perror("malloc matrice");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<n;i++){
        A[i]= malloc(n*sizeof(int));
        if(A[i]==NULL){
            perror("malloc righe");
            exit(EXIT_FAILURE);
        }
    }

    v = calloc(sizev, sizeof(int));
    if(v == NULL){
        perror("calloc v");
        exit(EXIT_FAILURE);
    }

    contatore = calloc(n,sizeof(int));
    if(contatore == NULL){
        perror("calloc contatore");
        exit(EXIT_FAILURE);
    }


    //riempio la matrice con numeri interi pseudocasuali
    srand(time(NULL));

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            A[i][j]=rand()%(30-10+1)+10;
        }
    }

   
    //stampa di debug matrice

    printf("\nMatrice di partenza: \n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d ", A[i][j]);
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

        if(pthread_create(&thread[i], NULL, prelievoelemento, arg)!=0){
            perror("pthread_create threads");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_create(&stampatore,NULL, stampavettore, NULL)!=0){
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
    if(pthread_join(stampatore,NULL)!=0){
        perror("pthread_join stampatore");
        exit(EXIT_FAILURE);
    }


    //dealloco memoria matrice e vettore
    for(int i=0;i<n;i++){
        free(A[i]);
    }
    free(A);
    free(v);
    free(contatore);

    return 0;
}
