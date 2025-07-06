/*Scrivere un programma che, presa una directory da riga di comando, provveda a scorrerla
allo scopo di stampare i nomi di tutti i file puntati da link simbolici e la loro dimensione,
se questi hanno i diritti di scrittura e lettura per il proprietario e per gli altri.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
        return 1;
    }

    DIR *dir = opendir(argv[1]);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    char linkpath[PATH_MAX], target[PATH_MAX], fullpath[PATH_MAX];
    struct stat sb;

    while ((entry = readdir(dir))!=NULL) {
        // Salta "." e ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        // linkpath = directory + nome entry (potenziale link simbolico)
        // Esempio:
        //   argv[1]   = "/home/user/test"
        //   d_name    = "link1"
        // → linkpath = "/home/user/test/link1"
        snprintf(linkpath, sizeof(linkpath), "%s/%s", argv[1], entry->d_name);

        // lstat → controlla se il file è un link simbolico (senza seguirlo)
        if (lstat(linkpath, &sb) == -1 || !S_ISLNK(sb.st_mode))
            continue;

        // Legge il target del link simbolico
        // Esempi:
        //   target = "../file.txt"      → relativo
        //   target = "/home/user/file"  → assoluto
        ssize_t len = readlink(linkpath, target, sizeof(target) - 1);
        if (len == -1)
            continue;
        target[len] = '\0'; // Aggiunge il terminatore

        // Costruisce il path completo al file puntato
        if (target[0] == '/') {
            // target assoluto → già pronto
            // Esempio:
            //   target   = "/home/user/file.txt"
            // → fullpath = "/home/user/file.txt"
            strncpy(fullpath, target, sizeof(fullpath));
        } else {
            // target relativo → va risolto rispetto alla directory del link
            // Esempio:
            //   linkpath = "/home/user/test/link1"
            //   target   = "../file.txt"
            // → dirname(linkpath) = "/home/user/test"
            // → fullpath = "/home/user/test/../file.txt" → "/home/user/file.txt"
            
            // strdup() crea una copia di linkpath perché dirname() modifica la stringa
            char *dircopy = strdup(linkpath);
            // dirname() estrae solo la parte di directory dal percorso (es. "/home/user/test/link1" → "/home/user/test")
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname(dircopy), target);
            free(dircopy); // libera la memoria allocata da strdup

        }

        // stat segue il link → verifica il file reale puntato
        if (stat(fullpath, &sb) == -1)
            continue;

        // Controlla se è un file regolare con permessi rw per utente e altri
        if (S_ISREG(sb.st_mode) &&
            (sb.st_mode & S_IRUSR) && (sb.st_mode & S_IWUSR) &&
            (sb.st_mode & S_IROTH) && (sb.st_mode & S_IWOTH)) {

            // basename prende solo il nome file finale
            // Esempio:
            //   fullpath = "/home/user/file.txt"
            // → basename = "file.txt"
            char *namecopy = strdup(fullpath);
            printf("Nome file: %s - Dimensione: %ld byte\n", basename(namecopy), sb.st_size);
            free(namecopy);
        }
    }

    closedir(dir);
    return 0;
}
