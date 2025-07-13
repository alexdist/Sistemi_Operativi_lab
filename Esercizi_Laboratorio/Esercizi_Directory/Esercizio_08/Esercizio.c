/*Scrivere un programma in C e Posix sotto Linux che, presa in input un file completo di percorso,
restituisca lo UID del proprietario, la grandezza del file e tutti i permessi associati a tale file*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(stderr, "USO %s <file_completo_di_percorso>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *path = argv[1];

    struct stat sb;

    if(stat(path, &sb)==-1){
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if(S_ISREG(sb.st_mode)){
        printf("UID Proprietario: %d - Dimensione file: %ld byte\n", sb.st_uid, sb.st_size);
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

    return 0;
}