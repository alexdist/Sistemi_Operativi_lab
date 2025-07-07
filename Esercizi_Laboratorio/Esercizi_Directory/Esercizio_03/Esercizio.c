/*Si realizzi un programma in C, secondo lo standard Posix, in ambiente Linux, 
che presa una directory da riga di comando come parametro, 
ne stampi i dettagli (nome, data di ultimo accesso, permessi) 
dei file puntati dai link simbolici presenti nella directory*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(stderr,"USO: %s <directory>", argv[0]);
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        perror("dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct stat sb;

    char linkpath[PATH_MAX];
    char target[PATH_MAX];
    char fullpath[PATH_MAX];

    while((entry = readdir(dir))!=NULL)
    {
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name,"..")==0){
            continue;
        }

        snprintf(linkpath, sizeof(linkpath), "%s/%s", argv[1], entry->d_name);

        if(lstat(linkpath, &sb)==-1 || !S_ISLNK(sb.st_mode)){ //se lstat fallisce o il file non è un link, fa ripartire il ciclo while
            continue;
        }

        ssize_t len = readlink(linkpath, target, sizeof(target)-1);
        if(len == -1){
            perror("len");
            continue;
        }
        target[len]='\0';

        if(target[0] == '/'){ //il percorso è assoluto
            
            strncpy(fullpath, target, sizeof(fullpath));
        }
        else{//il percorso è relativo, bisogna costruire il percorso assoluto

            char* dircopy = strdup(linkpath); //fa una copia di linkpath in dircopy perché dirname modifica la stringa
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname(dircopy), target);
            free(dircopy);
        } 

        if(stat(fullpath, &sb)==-1){
            perror("stat");
            continue;
        }

        if(S_ISREG(sb.st_mode)){

            printf("Nome file: %s - Dimensione: %ld byte - Data Ultimo Accesso: %s\n", basename(fullpath), sb.st_size, ctime(&sb.st_atime));
            printf("Permessi: \n");
            printf((sb.st_mode & S_IRUSR)? "r":"-");
            printf((sb.st_mode & S_IWUSR)? "w":"-");
            printf((sb.st_mode & S_IXUSR)? "x":"-");
            printf((sb.st_mode & S_IRGRP)? "r":"-");
            printf((sb.st_mode & S_IWGRP)? "w":"-");
            printf((sb.st_mode & S_IXGRP)? "x":"-");
            printf((sb.st_mode & S_IROTH)? "r":"-");
            printf((sb.st_mode & S_IWOTH)? "w":"-");
            printf((sb.st_mode & S_IXOTH)? "x":"-");
            printf("\n");
        }
     }
     closedir(dir);

     return 0;
}