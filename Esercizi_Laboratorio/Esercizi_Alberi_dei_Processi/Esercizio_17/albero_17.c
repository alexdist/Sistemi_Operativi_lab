// ALBERO 17
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 5;
    int pid  = 1;
    int counter = 2;

    while (counter > 0) {
        if (fork()) {
            glob -= 3;
        } else {
            glob--;
            if (fork() == 0) {
                if (pid > 0) {
                    pid = fork();
                    glob--;
                }
            }
        }
        counter--;
    }

    printf("Valore di glob = %d\n", glob);

    return 0;
}
