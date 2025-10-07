// ALBERO 6
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 5;
    int pid  = 0;

    pid = fork();
    glob--;

    pid = fork();
    glob--;

    if (pid != 0) {
        pid = fork();
        glob--;
    }

    printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);

    return 0;
}
