// ALBERO 9
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 5;
    int pid  = 0;
    int i;

    if (fork()) {
        if (pid) {
            glob = glob - 1;
        } else {
            for (i = 0; i < glob - 3; i++) {
                pid = fork();

                if (!pid)
                    glob = glob - 1;
                else
                    pid = 0;
            }
        }
    }

    printf("Valore di glob: %d | Valore di pid: %d | i: %d\n", glob, pid, i);

    return 0;
}
