// ALBERO 3
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 4;
    int pid  = 1;

    fork();
    glob++;

    if (!fork()) {
        pid = fork();
        glob = glob * 2;
        pid = 0;  // corretto da 'Pid'
    }

    if (!pid) {
        glob = glob - 2;
    } else {
        fork();
        glob++;
    }

    printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);

    return 0;
}
