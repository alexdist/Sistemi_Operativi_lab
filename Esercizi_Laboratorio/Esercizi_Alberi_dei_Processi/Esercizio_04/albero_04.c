// ALBERO 4
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int glob = 3;
    int pid  = 3;
    int var  = 3;

    var = fork();
    glob--;

    if (var) {
        pid = fork();
        glob++;
    }

    if (pid) {
        glob--;
        var--;
    }

    if (!glob) {
        var = fork();
        pid++;
    }

    printf("Valore di glob: %d | Valore di pid: %d | Valore di var: %d\n", glob, pid, var);

    return 0;
}
