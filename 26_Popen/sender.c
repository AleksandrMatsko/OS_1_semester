#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Sender: wrong amount of arguments\n");
        exit(1);
    }
    int fd_to_read = open(argv[1], O_RDONLY);
    if (fd_to_read == -1) {
        perror("Sender: error while open file to send");
        exit(2);
    }
    char buffer[BUFSIZ] = "";
    while (1) {
        ssize_t was_read = read(fd_to_read, buffer, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("Sender: error while reading");
            exit(3);
        }
        ssize_t was_write = write(1, buffer, was_read);
        if (was_write == -1) {
            perror("Sender: error while writing to out");
            exit(4);
        }
    }
    if (close(fd_to_read) != 0) {
        perror("Sender: error while close file to send");
        exit(5);
    }
    return 0;
}