/*Scrivere un programma in ambiente Linux che implementi, stile shell BASH, la seguente pipeline
inserita da riga di comando: ls -LF | wc -c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];

    if(pipe(pipefd)==-1){
        fprintf(stderr, "Errore nella creazione della pipe\n");
        exit(EXIT_FAILURE);
    }


    //Primo figlio: esegue "ls -LF"
    pid_t pid1 = fork();

    if(pid1<0){
        fprintf(stderr, "Errore nella fork del primo processo\n");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){
        //Reindirizza STDOUT sulla scrittura della pipe
        if(dup2(pipefd[1], STDOUT_FILENO) == -1){
            perror("dup2 ls");
            exit(EXIT_FAILURE);
        }

        //Chiude entrambi gli estremi della pipe (non servono più nel figlio)
        close(pipefd[0]);
        close(pipefd[1]);

        char *cmd[] ={"ls", "-LF", NULL};
        execvp(cmd[0], cmd);
        perror("execvp ls");
        exit(EXIT_FAILURE);
    }

    //Secondo figlio: esegue "wc -c"
    pid_t pid2 = fork();

    if(pid2<0){
        fprintf(stderr, "Errore nella fork del secondo processo\n");
        exit(EXIT_FAILURE);
    }
    else if(pid2 == 0){
        //Reindirizza STDIN sulla lettura della pipe
        if(dup2(pipefd[0], STDIN_FILENO)==-1){
            perror("dup2 ls");
            exit(EXIT_FAILURE);
        }
        
        //Chiude entrambi gli estremi della pipe (non servono più nel figlio)
        close(pipefd[1]);
        close(pipefd[0]);

        char *cmd2[] = {"wc", "-c", NULL};
        execvp(cmd2[0], cmd2);
        perror("execvp wc");
        exit(EXIT_FAILURE);

    }

    //Processo padre: chiude entrambi gli estremi della pipe
    close(pipefd[0]);
    close(pipefd[1]);

    //processo padre attende entrambi i figli
    if(waitpid(pid1, NULL, 0)==-1){
        perror("waitpid pid1");
    }

    if(waitpid(pid2, NULL, 0)==-1){
        perror("waitpid pid2");
    }
    
    return 0;
}