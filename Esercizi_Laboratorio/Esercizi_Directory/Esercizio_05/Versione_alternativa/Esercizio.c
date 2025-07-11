/*Scrivere un programma in C e Posix sotto Linux che, presa in input una directory, dopo averne
analizzato il suo contenuto, identifichi il file di lunghezza minima e ne stampi la maschera
dei permessi relativi agli altri.*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "USO: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    DIR* dir = opendir(argv[1]);
    if(dir == NULL)
    {
        perror("dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct dirent *min_entry;
    struct stat sb;
    long min_size = -1;
    mode_t min_mode = 0;
    
    char target[4096];

    while((entry = readdir(dir))!= NULL)
    {   
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
        {continue;}

        snprintf(target, sizeof(target), "%s/%s", argv[1], entry->d_name);

        if(stat(target, &sb)==-1)
        {continue;}

        if(S_ISREG(sb.st_mode)){
            if(min_size == -1 || sb.st_size < min_size){
                
                min_size = sb.st_size; //salvo il size del file di lunghezza minima trovato fin'ora
                min_mode = sb.st_mode;  //faccio lo stesso con i permessi
                min_entry = entry;      //faccio una cosa in più: salvo anche il nome del file di lunghezza minima.
            }
        }
    }
    closedir(dir);

    if(min_size == -1){
        printf("Nessun file trovato nella directory\n");
    }else{
        printf("Il file di lunghezza minima è: %s - Dimensione %lu byte", min_entry->d_name, min_size);
        printf("\nPermessi per gli altri: \n");
        printf(min_mode & S_IROTH ? "r":"-");
        printf(min_mode & S_IWOTH ? "w":"-");
        printf(min_mode & S_IXOTH ? "x":"-\n");
    }
    return EXIT_SUCCESS;

}