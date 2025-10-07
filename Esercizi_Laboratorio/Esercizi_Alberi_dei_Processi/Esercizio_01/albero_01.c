#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int Glob = 0;
    int Pid = 0;

    while (fork()) {
        Glob += 1;

        if (!fork()) {
            Pid = 1;
        } else {
            fork();
            Glob++;
        }

        if (Glob > 1) {
            printf("Valore di Glob: %d | Valore di Pid: %d\n", Glob, Pid);
            exit(0);
        }
    }

    return 0;
}

