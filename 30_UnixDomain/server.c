#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

char *socket_path = "./socket";

int main() {
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error socket()");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    int bind_res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (bind_res == -1) {
        perror("Error bind()");
        exit(2);
    }

    int listen_res = listen(socket_fd, 1);
    if (listen_res == -1) {
        perror("Error listen");
        exit(3);
    }

    printf("Server started\n");

    int client_fd = accept(socket_fd, NULL, NULL);
    unlink(addr.sun_path);
    if (client_fd == -1) {
        perror("Error accept");
        exit(4);
    }

    printf("Client connected\n");
    ssize_t was_read;
    char buf[BUFSIZ] = "";

    while (1) {
        was_read = read(client_fd, buf, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("Error read() from socket");
            exit(5);
        }
        for (ssize_t i = 0; i < was_read; i++) {
            buf[i] = toupper(buf[i]);
        }
        ssize_t written = write(fileno(stdout), buf, was_read);
        if (written == -1) {
            perror("Error write()");
            exit(6);
        }
    }

    close(client_fd);
    printf("\nDisconnect client\n");
    close(socket_fd);
    return 0;
}
