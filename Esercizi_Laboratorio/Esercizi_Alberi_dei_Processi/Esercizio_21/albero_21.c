// ALBERO 21
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 5;
    int pid  = 1;
    int i;

    fork();

    if (!(pid = fork()))
        fork();

    for (i = 0; i < 2; i++) {
        if (pid) {
            fork();
            glob++;
            i++;
        } else {
            glob = glob * 2;
        }
        glob++;
    }

    printf("Valore di glob: %d | Valore di pid: %d\n", glob, pid);

    return 0;
}
