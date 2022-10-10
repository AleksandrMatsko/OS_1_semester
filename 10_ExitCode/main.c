#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments. Expected at least 2, has %d\n", argc);
        exit(1);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error in fork");
        exit(2);
    }
    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "exec() failure\n");
        exit(3);
    }
    int status;
    pid_t ret = wait(&status);
    if (ret < 0) {
        perror("Error in wait()");
        exit(4);
    }
    if (WIFEXITED(status)) {
        printf("exit status is %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        printf("signal is %d\n", WTERMSIG(status));
    }
    return 0;
}
