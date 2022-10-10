#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char *socket_path = "./socket";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error: wrong amount of arguments\n");
        exit(1);
    }

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error socket()");
        exit(2);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    int connect_res = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (connect_res == -1) {
        perror("Error connect()");
        exit(3);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error open() file");
        exit(4);
    }

    char buf[BUFSIZ] = "";

    while (1) {
        ssize_t was_read = read(fd, buf, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("Error read() file");
            exit(5);
        }
        ssize_t written = write(socket_fd, buf, was_read);
        if (written == -1) {
            perror("Error write() to socket");
            exit(6);
        }
    }

    close(fd);
    close(socket_fd);
    return 0;
}
