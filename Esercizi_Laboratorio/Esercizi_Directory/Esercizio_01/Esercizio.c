/*Scrivere un programma in C che, in ambiente Linux, presa una directory come parametro
da riga di comando, provveda a scorrerla allo scopo di stampare i nomi di tutti i file
regolari e le loro dimensioni contenuti in tale directory se questi hanno i diritti di
scrittura e lettura per il proprietario e per gli altri*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc, char* argv[])
{
    //Controlla che l'utente abbia passato esattamente un argomento (la directory)
    if(argc!=2){
        fprintf(stderr, "USO: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Apre la directory specificata
    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        perror("dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry; //Puntatore per scorrere le entry della directory
    
    char target[4096]; //Buffer per il path completo del file

    //Ciclo che legge tutte le entry della directory
    while((entry = readdir(dir))!= NULL)
    {
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){ //ignora le entry speciali "." e ".."
            continue;
        }

        //Costruisce il path completo del file: <directory>/<nomefile>
        snprintf(target, sizeof(target), "%s/%s", argv[1], entry->d_name);

        
        struct stat sb; //Struct per contenere informazioni sul file

        //Ottiene informazioni sul file tramite stat()
        if(stat(target, &sb)==-1){
            perror("stat");
            continue;
        }

        //Controlla che sia un file regolare
        //E che abbia i permessi: lettura e scrittura per il proprietario (USR)
        //E lettura e scrittura per gli altri (OTH)
        if(S_ISREG(sb.st_mode) && (sb.st_mode & S_IRUSR) && (sb.st_mode & S_IWUSR) 
                && (sb.st_mode & S_IROTH)&&(sb.st_mode & S_IWOTH)){

                    printf("Nome file: %s - Dimensione: %ld\n", entry->d_name, sb.st_size);
                }
    }
    //Chiude la directory dopo averla scorsa tutta
    closedir(dir);

    return 0;
}