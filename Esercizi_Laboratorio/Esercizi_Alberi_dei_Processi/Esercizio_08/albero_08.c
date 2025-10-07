// ALBERO 8
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 6;
    int pid  = 0;
    int i;

    fork();

    for (i = 1; i < 2; i++) {
        if (!fork()) {
            glob = glob * 2;
            pid = 1;
        }

        if (pid) {
            fork();
        }

        glob = glob - 1;
    }

    printf("Valore di glob: %d | Valore di pid: %d | i: %d\n", glob, pid, i);

    return 0;
}
