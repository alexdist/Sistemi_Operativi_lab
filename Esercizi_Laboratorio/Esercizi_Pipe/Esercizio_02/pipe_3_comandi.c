/*Si scriva un programma in C sotto Linux che implementi la seguente pipeline di comandi SHELL: 
 ls -la | sort | tail -10*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/wait.h>

 int main()
 {
    int pipefd1[2], pipefd2[2];

    if(pipe(pipefd1)==-1){
        fprintf(stderr,"Errore nella creazione della prima pipe\n");
        exit(EXIT_FAILURE);
    }

    if(pipe(pipefd2)==-1){
        fprintf(stderr, "Errore nella creazione della seconda pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();

    if(pid1 < 0){
        fprintf(stderr, "Errore nella fork del primo processo\n");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){

        if(dup2(pipefd1[1], STDOUT_FILENO)==-1){
            fprintf(stderr, "Errore dup2 del primo processo\n");
            exit(EXIT_FAILURE);
        }

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        char *cmd1[]={"ls", "-la", NULL};
        execvp(cmd1[0], cmd1);
        perror("execvp ls");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();

    if(pid2 < 0){
        fprintf(stderr, "Errore nella fork del secondo processo\n");
        exit(EXIT_FAILURE);
    }
    else if(pid2 == 0){

        if(dup2(pipefd1[0], STDIN_FILENO)==-1){
            fprintf(stderr, "Errore dup2 del secondo processo\n");
            exit(EXIT_FAILURE);
        }

        if(dup2(pipefd2[1], STDOUT_FILENO)==-1){
            fprintf(stderr, "Errore dup2 del secondo processo\n");
            exit(EXIT_FAILURE);
        }

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        char *cmd2[]={"sort", NULL};
        execvp(cmd2[0], cmd2);
        perror("execvp sort");
        exit(EXIT_FAILURE);
    }

    pid_t pid3 = fork();

    if(pid3 < 0){
        fprintf(stderr, "Errore nella creazione del terzo processo\n");
        exit(EXIT_FAILURE);
    }
    else if(pid3 == 0){

        if(dup2(pipefd2[0], STDIN_FILENO)==-1){
            fprintf(stderr, "Errore dup2 del terzo processo\n");
            exit(EXIT_FAILURE);
        }

        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        char *cmd3[]={"tail", "-10", NULL};
        execvp(cmd3[0], cmd3);
        perror("execvp tail");
        exit(EXIT_FAILURE);
    }

    //processo padre
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    if(waitpid(pid1, NULL, 0)==-1){
        fprintf(stderr,"Errore waitpid padre del primo processo figlio\n");
        exit(EXIT_FAILURE);
    }

    if(waitpid(pid2, NULL, 0)==-1){
        fprintf(stderr, "Errore waitpid padre del secondo processo figlio\n");
        exit(EXIT_FAILURE);
    }

    if(waitpid(pid3, NULL, 0)==-1){
        fprintf(stderr, "Errore waitpid padre del terzo processo figlio\n");
        exit(EXIT_FAILURE);
    }

    return 0;



 }