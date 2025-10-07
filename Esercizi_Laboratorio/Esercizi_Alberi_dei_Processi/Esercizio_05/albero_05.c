// ALBERO 5
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 20;
    int pid  = 0;

    for (int i = 2; i < 4; i++) {
        pid = fork();
        if (pid == 0) {
            glob = glob * 2;
            sleep(i + 1);
        }

        glob = glob - 1;
        printf("i=%d | Valore di glob: %d | Valore di pid: %d\n", i, glob, pid);
    }

    return 0;
}
