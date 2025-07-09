/*Si scriva un programma in C sotto Linux che implementi la seguente pipeline di comandi:
ls -la/ | sort | tail -10 | wc -l*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){

    int pipefd1[2], pipefd2[2], pipefd3[2];

    if(pipe(pipefd1)==-1 || pipe(pipefd2)==-1 || pipe(pipefd3)==-1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //Processo 1: ls -la
    pid_t pid1 = fork();
    if(pid1<0){
        perror("fork pid1");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){

        dup2(pipefd1[1], STDOUT_FILENO);
        
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        close(pipefd3[0]);
        close(pipefd3[1]);

        char *cmd1[]={"ls", "-la", "/", NULL};
        execvp(cmd1[0], cmd1);
        perror("execvp ls");
        exit(EXIT_FAILURE);
    }

    //Processo 2: sort
    pid_t pid2 = fork();
    if(pid2<0){
        perror("fork pid2");
        exit(EXIT_FAILURE);
    }
    else if(pid2 == 0){
        
        dup2(pipefd1[0], STDIN_FILENO);
        dup2(pipefd2[1], STDOUT_FILENO);

        close(pipefd1[1]);
        close(pipefd1[0]);
        close(pipefd2[1]);
        close(pipefd2[0]);
        close(pipefd3[1]);
        close(pipefd3[0]);

        char *cmd2[] = {"sort", NULL};
        execvp(cmd2[0], cmd2);
        perror("execvp sort");
        exit(EXIT_FAILURE);
    }

    //Processo 3: tail -10
    pid_t pid3 = fork();
    if(pid3<0){
        perror("fork pid3");
        exit(EXIT_FAILURE);
    }
    else if(pid3 == 0){

        dup2(pipefd2[0], STDIN_FILENO);
        dup2(pipefd3[1], STDOUT_FILENO);

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        close(pipefd3[0]);
        close(pipefd3[1]);

        char *cmd3[]={"tail", "-10", NULL};
        execvp(cmd3[0], cmd3);
        perror("execvp tail");
        exit(EXIT_FAILURE);
    }

    //Processo 4: wc -l
    pid_t pid4 = fork();
    if(pid4<0){
        perror("fork pid4");
        exit(EXIT_FAILURE);
    }
    else if(pid4 == 0){

        dup2(pipefd3[0],STDIN_FILENO);

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        close(pipefd3[0]);
        close(pipefd3[1]);

        char *cmd4[]={"wc", "-l", NULL};
        execvp(cmd4[0], cmd4);
        perror("execvp wc");
        exit(EXIT_FAILURE);
    }


    //Processo padre: chiude tutto e aspetta

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        close(pipefd3[0]);
        close(pipefd3[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        waitpid(pid3, NULL, 0);
        waitpid(pid4, NULL, 0);



    return 0;
}


