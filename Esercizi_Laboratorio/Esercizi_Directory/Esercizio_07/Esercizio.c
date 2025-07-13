/*Scrivere un programma in C e Posix sotto Linux che, presa in input un file completo di percorso, 
verifichi se questo sia un link e a quale file punta e infine del file puntato
restituire lo UID del proprietario, la grandezza del file e tutti i permessi associati a tale file*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <errno.h>
#include <linux/limits.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USO: %s <percorso_completo_del_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *inputpath = argv[1];
    char link_target[PATH_MAX];
    struct stat sb;

    // Usa lstat per verificare se è un link simbolico
    if (lstat(inputpath, &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if (!S_ISLNK(sb.st_mode)) {
        printf("Il file selezionato NON è un link simbolico.\n");
        return 0;
    }

    // Leggi il contenuto del link (cioè il path del file puntato)
    ssize_t len = readlink(inputpath, link_target, sizeof(link_target) - 1);
    if (len == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    link_target[len] = '\0';

    // Costruisci il path assoluto se il link è relativo
    char resolved_path[PATH_MAX];
    if (link_target[0] == '/') {
        // È già assoluto
        strncpy(resolved_path, link_target, PATH_MAX);
    } else {
        // Relativo: costruisci il path rispetto alla directory del link
        char input_copy[PATH_MAX];
        strncpy(input_copy, inputpath, PATH_MAX);
        char *dir = dirname(input_copy); //per ottenere la directory in cui si trova il link
        //Es. dirname("/home/alex/links/mylink") → ritorna "/home/alex/links"

        snprintf(resolved_path, PATH_MAX, "%s/%s", dir, link_target); //unisce la directory del link con il path relativo del target.
        /*ES. inputpath: /home/alex/links/mylink

        link_target (contenuto del link): ../data/file.txt

        dirname(inputpath): /home/alex/links

        resolved_path: /home/alex/links/../data/file.txt*/
    }

    // Ottieni informazioni sul file puntato
    if (stat(resolved_path, &sb) == -1) {
        perror("stat sul file puntato");
        exit(EXIT_FAILURE);
    }

    // Stampa informazioni richieste
    printf("\nIl file è un link simbolico.\n");
    printf("Il link punta a: %s\n", resolved_path);
    printf("UID del proprietario: %d\n", sb.st_uid);
    printf("Dimensione del file: %ld byte\n", sb.st_size);
    printf("Permessi: ");
    printf((sb.st_mode & S_IRUSR) ? "r" : "-");
    printf((sb.st_mode & S_IWUSR) ? "w" : "-");
    printf((sb.st_mode & S_IXUSR) ? "x" : "-");
    printf((sb.st_mode & S_IRGRP) ? "r" : "-");
    printf((sb.st_mode & S_IWGRP) ? "w" : "-");
    printf((sb.st_mode & S_IXGRP) ? "x" : "-");
    printf((sb.st_mode & S_IROTH) ? "r" : "-");
    printf((sb.st_mode & S_IWOTH) ? "w" : "-");
    printf((sb.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    return 0;
}
