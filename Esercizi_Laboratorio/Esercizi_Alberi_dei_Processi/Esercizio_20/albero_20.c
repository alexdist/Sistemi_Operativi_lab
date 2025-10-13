// ALBERO 20
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 0;
    int pid  = 0;

    while (!fork()) {
        glob += 1;

        if (fork()) {
            pid = 1;
        } else {
            fork();
            glob++;
        }

        if (glob > 1) {
            printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);
            exit(0);
        }
    }

    return 0;
}
