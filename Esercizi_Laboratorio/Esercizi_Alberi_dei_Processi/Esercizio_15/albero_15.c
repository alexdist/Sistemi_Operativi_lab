// ALBERO 15
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 2;
    int pid  = 1;

    fork();
    pid = fork();

    if (pid) {
        while (glob) {
            glob--;
            pid = fork();
        }
    } else {
        if (fork())
            glob = 10;
    }

    printf("Valore di glob = %d\n", glob);

    return 0;
}
