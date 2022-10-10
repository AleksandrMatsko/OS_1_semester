#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

void closePipe(FILE *pipe_file) {
    int res = pclose(pipe_file);
    if (res == -1) {
        perror("Main: error in pclose()");
        exit(3);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Main: wrong amount of arguments\n");
        exit(1);
    }

    const char *command_start = "./sender.out ";
    unsigned int length = strlen(command_start) + strlen(argv[1]) + 1;
    char *command = (char *)calloc(length, sizeof(char));
    memcpy(command, command_start, strlen(command_start) * sizeof(char));
    memcpy(&command[strlen(command_start)], argv[1], (strlen(argv[1]) + 1) * sizeof(char));

    FILE *pipe_file = popen(command, "r");
    if (pipe_file == NULL) {
        perror("Main: error in popen()");
        exit(2);
    }

    char buffer[BUFSIZ] = "";
    while (!feof(pipe_file)) {
        errno = 0;
        size_t was_read = fread(buffer, sizeof(char), BUFSIZ, pipe_file);
        if (errno != 0) {
            perror("Main: error while reading data from sender");
            closePipe(pipe_file);
            exit(4);
        }
        for (size_t i = 0; i < was_read; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        size_t was_write = fwrite(buffer, sizeof(char), was_read, stdout);
        if (was_write != was_read) {
            perror("Main: error while writing");
            closePipe(pipe_file);
            exit(5);
        }
    }


    closePipe(pipe_file);
    free(command);
    return 0;
}
