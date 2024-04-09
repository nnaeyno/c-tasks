#include <sys/types.h>     
#include <sys/wait.h>      
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      
#include <fcntl.h> 

int execute_prog(char *prog){
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        printf("A fork error has occurred.\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        /* the child */
        // printf("I am the child, calling execlp.\n");
        execlp(prog, prog, NULL);
        /*  If execlp() is successful, we won't reach this */
        printf("The call to execlp() was not successful.\n");
        exit(EXIT_FAILURE);
    } else {
        /* parent */
        int status;
        
        waitpid(pid, &status, 0);
    
        if ( WIFEXITED(status) ) 
        {
            int exit_status = WEXITSTATUS(status);        
            if (exit_status){
                return (EXIT_FAILURE);
            }
        }
    }

    return(0);  
}


int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "Usage: %s prog1 prog2 prog3 file\n", argv[0]);
        return 1;
    }

    char *prog1 = argv[1];
    char *prog2 = argv[2];
    char *prog3 = argv[3];
    char *file  = argv[4];   
  
    if (execute_prog(prog1)){
        return (1);
    }
    // printf("SUCCESS\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid_prog2 = fork();
    if (pid_prog2 == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid_prog2 == 0) {
        // Child process for prog2
        close(pipefd[0]); // Close read end of pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        execlp(prog2, prog2, NULL); // Execute ls
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid_prog3 = fork();
    if (pid_prog3 == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid_prog3 == 0) {
        // Child
        close(pipefd[1]); // Close write end of pipe
        int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file for writing
        if (fd == -1) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin from pipe
        dup2(fd, STDOUT_FILENO); // Redirect stdout to file
        execlp(prog3, prog3, NULL); // Execute prog3
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    // Wait to finish
    waitpid(pid_prog2, NULL, 0);
    waitpid(pid_prog3, NULL, 0);

    // printf("prog1 && prog2 | prog3 > file executed successfully.\n");

    return (0);

}