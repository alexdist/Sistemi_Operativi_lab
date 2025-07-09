/*Scrivere un programma in C (POSIX, LINUX) che, dati in input un numero intero e il percorso
di una directory, stampi il nome e la dimensione di tutti i file presenti nella directory
la cui dimensione supera il valore numerico specificato*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>


int main(int argc, char *argv[])
{
    if(argc!=3){
        fprintf(stderr, "USO: %s <directory> <integer(x)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    long x = atoi(argv[2]);

    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        perror("dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct stat sb;

    char target[PATH_MAX];
    
    while((entry = readdir(dir))!= NULL)
    {
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){ //ignora "." e ".."
            continue;
        }

        snprintf(target, sizeof(target), "%s/%s", argv[1], entry->d_name);

        if(stat(target, &sb)==-1){
            perror("stat");
            continue;
        }

        if(S_ISREG(sb.st_mode) && (sb.st_size > x)){

            printf("Nome file: %s - Dimensione: %ld byte\n\n", entry->d_name, sb.st_size);
        }
    }
    closedir(dir);

    return 0;

}