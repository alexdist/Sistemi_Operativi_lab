// ALBERO 11
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 6;
    int pid  = 0;

    if (!(pid = fork())) {
        fork();
        glob--;
    }

    fork();
    glob++;

    if (pid)
        fork();
    else
        glob--;

    printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);

    return 0;
}
