/*Scrivere un programma in C e Posix sotto Linux che, presa in input una directory, dopo averne
analizzato il suo contenuto, identifichi il file di lunghezza minima e ne stampi la maschera
dei permessi relativi agli altri.*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h> //per LLONG_MAX
#include <libgen.h> //per basename()

int main(int argc, char* argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Apertura della directory
    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        perror("dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct stat sb;
    struct stat sb2;
    off_t minsize = LLONG_MAX; //Inizializza al massimo valore possibile

    char target[4096]; //Percorso completo del file corrente
    char shortfile[4096]; //Percorso del file più piccolo trovato

    while((entry = readdir(dir))!= NULL)
    {
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name,"..")==0){ //ignora "." e ".."
            continue;
        }

        //Costruisce il percorso assoluto del file
        snprintf(target, sizeof(target), "%s/%s", argv[1], entry->d_name);

        //Ottiene informazioni sul file
        if(stat(target, &sb)==-1){
            perror("stat"); //se stat fallisce, salta al prossimo file
            continue;
        }

        //Verifica se è un file regolare e se ha dimensione minima finora
        if(S_ISREG(sb.st_mode) && sb.st_size < minsize){
            minsize = sb.st_size;
            snprintf(shortfile, sizeof(shortfile), "%s/%s", argv[1], entry->d_name);
        }
        
    }
    closedir(dir); //chiude la directory

    //Nessun file regolare trovato nella directory
    if(minsize == LLONG_MAX){
        printf("Nessun file regolare trovato nella directory\n");
        return EXIT_SUCCESS;
    }
        //Ottiene di nuovo le info sul file più piccolo per stampare i permessi
        if(stat(shortfile, &sb2)==-1){
            perror("stat2");
            exit(EXIT_FAILURE);
        }
        //Stampa il nome del file (basename estrae solo il nome, senza percorso)
        printf("Nome file: %s\n", basename(shortfile));
        
        printf("Permessi relativi agli altri: \n");
        printf((sb2.st_mode & S_IROTH)? "r":"-");
        printf((sb2.st_mode & S_IWOTH)? "w":"-");
        printf((sb2.st_mode & S_IXOTH)? "x":"-");
        printf("\n");

   return 0;

}