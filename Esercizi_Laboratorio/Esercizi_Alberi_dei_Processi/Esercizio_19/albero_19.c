// ALBERO 19
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 10;
    int pid  = 1;
    int i;

    for (i = 1; i < 3; i++) {
        glob = glob - i;
        fork();

        if (!pid)
            pid = fork();

        if (pid)
            glob -= 2;

        glob -= 1;
    }

    if (!(pid = fork()))
        glob--;

    printf("Valore di glob = %d\n", glob);

    return 0;
}
