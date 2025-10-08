// ALBERO 14
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 6;
    int pid  = 0;

    while (!pid) {
        if (!fork()) {
            glob -= 3;
        } else {
            glob--;
            if ((pid = fork()) == 0) {
                pid = 1;
                fork();
                glob--;
            }
        }

        if (!glob)
            pid = 10;
    }

    printf("Valore di glob = %d\n", glob);

    return 0;
}
