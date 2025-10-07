// ALBERO 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 2;
    int pid = 0;
    int i;

    for (i = 0; i < 3; i++) {
        fork();

        if (!pid) {
            glob = glob * pid;
            pid = fork();
        }

        glob = glob - 1;
    }

    if (glob > 0) {
        printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);
        exit(0);
    }

    return 0;
}
