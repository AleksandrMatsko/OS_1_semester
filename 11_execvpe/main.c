#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char *filename, char *argv[], char *envp[]) {
    char **prev_environ = environ;
    environ = envp;
    execvp(filename, argv);
    environ = prev_environ;
    return -1;
}

int main() {
    char *new_argv[] = {"test.out", "1", "parm2", "parm3", (char *) 0};
    char *new_envp[] = {/*"PATH=/home/students/20200/a.matsko/lab_works/11_execvpe/",*/"MyVar1=25", "MYVAR2=1", (char *) 0};
    execvpe(new_argv[0], new_argv, new_envp);
    perror("Error: execvpe failed");
    return 2;
}
