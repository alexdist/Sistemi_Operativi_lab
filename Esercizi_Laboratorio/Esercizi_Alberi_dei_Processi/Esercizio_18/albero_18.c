// ALBERO 18
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 2;
    int pid  = 0;
    int i;

    for (i = 1; i < 3; i++) {
        pid = fork();

        if (pid == 0) {
            glob = glob * 2;
            pid = fork();
        }

        glob = glob + 1;
    }

    printf("Valore di glob = %d\n", glob);

    return 0;
}
