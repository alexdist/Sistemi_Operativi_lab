/*Si realizzi un programma in C e Posix sotto Linux che presi due parametri da riga di comando
(il primo parametro sarà il percorso di una directory e il secondo sarà un numero intero che 
rappresenta i permessi nel formato ottale relativi ai file) faccia il parsing della directory
passato come primo argomento e stampi i nomi, i permessi e la data di ultima modifica dei file
contenuti in tale directory per cui i permessi corrispondono a quanto specificato nel secondo parametro.

Ad esempio, se il programma viene lanciato in questo modo:
./a.out  /usr/local/bin  4755
il programma stamperà il nome, i permessi e la data di ultima modifica di ogni file presente nella directory
/usr/local/bin che avranno i permessi seguenti: bit di set-uid settato, lettura + scrittura + esecuzione per il
proprietario, lettura + esecuzione sia per il gruppo che per tutti gli altri utenti del sistema.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    if(argc!=3){
        fprintf(stderr, "USO: %s <directory> <integer>");
        exit(EXIT_FAILURE);
    }
    
    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        perror("dir");
        exit(EXIT_FAILURE);
    }

    int expected_mode = strtol(argv[2], NULL, 8); //strtol permette di specificare la base (8 per ottale).
                                                  //converte la stringa ad es. 4755 in base ottale.

    struct dirent *entry;
    struct stat sb;
   

    char target[4096];

    while((entry = readdir(dir))!=NULL){

        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }

        snprintf(target, sizeof(target), "%s/%s", argv[1], entry->d_name);

        if(stat(target, &sb)==-1){
            perror("stat");
            continue;
        }
         mode_t file_mode = sb.st_mode & 07777;

        if(file_mode == expected_mode){
            printf("Nome file: %s - Data Ultima Modifica: %s\n", basename(target), ctime(&sb.st_mtime));

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

}