// ALBERO 7
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 5;
    int pid  = 0;
    int i = 0;

    for (i = 1; i < 3; i++) {
        pid = fork();

        if (pid == 0) {
            glob = glob * 2;
            sleep(i + 1);
        }

        glob = glob + 1;
    }

    printf("Valore di glob: %d | Valore di pid: %d | i: %d\n", glob, pid, i);

    return 0;
}
