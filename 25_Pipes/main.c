#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong amount of arguments\n");
        exit(1);
    }

    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) {
        perror("Error in pipe");
        exit(2);
    }

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("Error in fork");
        exit(3);
    }
    else if (pid == 0) { //child
        if (close(pipe_fds[1]) != 0) {
            perror("Child: error while closing other end of pipe");
            exit(4);
        }
        int fd_to_read = open(argv[1], O_RDONLY);
        if (fd_to_read == -1) {
            perror("Child: error while open file to send");
            exit(5);
        }
        char buffer[BUFSIZ] = "";
        while (1) {
            ssize_t was_read = read(fd_to_read, buffer, BUFSIZ);
            if (was_read == 0) {
                break;
            }
            else if (was_read == -1) {
                perror("Child: error while reading");
                exit(6);
            }
            ssize_t was_write = write(pipe_fds[0], buffer, was_read);
            if (was_write == -1) {
                perror("Child: error while writing to pipe");
                exit(7);
            }
        }
        if (close(fd_to_read) != 0) {
            perror("Child: error while close file to send");
            exit(8);
        }
        if (close(pipe_fds[0]) != 0) {
            perror("Child: error while closing end of pipe");
            exit(9);
        }
        return 0;
    }

    if (close(pipe_fds[0]) != 0) {
        perror("Parent: error while closing other end of pipe");
        exit(4);
    }
    char buffer[BUFSIZ] = "";
    while (1) {
        ssize_t was_read = read(pipe_fds[1], buffer, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("Parent: error while reading from pipe");
            break;
        }
        for (ssize_t i = 0; i < was_read; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        ssize_t was_write = write(1, buffer, was_read);
        if (was_write == -1) {
            perror("Parent: error while writing the result");
            break;
        }
    }
    if (close(pipe_fds[1]) != 0) {
        perror("Parent: error while closing pipe");
    }
    int status;
    pid_t ret = wait(&status);
    if (ret < 0) {
        perror("Parent: error in wait()");
        exit(5);
    }

    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0) {
            printf("Parent: child exit status is %d\n", exit_status);
        }
    }
    else if (WIFSIGNALED(status)) {
        printf("Parent: child signal is %d\n", WTERMSIG(status));
    }

    return 0;
}
