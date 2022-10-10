#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>

char *socket_path = "./socket";


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error wrong amount of arguments\n");
        exit(1);
    }

    int MAX_NUM_CLIENTS = atoi(argv[1]);
    int TIMEOUT = atoi(argv[2]);

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error socket()");
        exit(2);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    int bind_res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (bind_res == -1) {
        perror("Error bind()");
        exit(3);
    }

    int listen_res = listen(socket_fd, MAX_NUM_CLIENTS);
    if (listen_res == -1) {
        perror("Error listen");
        exit(4);
    }

    printf("#--Server started--#\n");

    struct pollfd *fds = calloc(MAX_NUM_CLIENTS + 1, sizeof(struct pollfd));
    if (fds == NULL) {
        perror("Error calloc()");
        exit(5);
    }
    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;


    ssize_t was_read;
    char buf[BUFSIZ] = "";
    int poll_count;
    int clients_count = 0;
    int total_clients_processed = 0;

    while (1) {
        poll_count = poll(fds, clients_count + 1, TIMEOUT);

        if (poll_count == -1) {
            perror("Error poll()");
            exit(6);
        }
        else if (poll_count == 0) {
            printf("Server timeout");
            break;
        }

        for (int i = 0; i < clients_count + 1; i++) {
            if (fds[i].revents & POLLIN) {
                if (i == 0 && clients_count < MAX_NUM_CLIENTS) {
                    int client_fd = accept(socket_fd, NULL, NULL);
                    if (client_fd == -1) {
                        perror("Error accept");
                        exit(7);
                    }
                    clients_count += 1;
                    fds[clients_count].fd = client_fd;
                    fds[clients_count].events = POLLIN;
                    total_clients_processed += 1;
                    printf("#--Client connected %d--#\n", clients_count);
                }
                else if (i != 0) {
                    was_read = read(fds[i].fd, buf, BUFSIZ);
                    if (was_read == 0) {
                        close(fds[i].fd);
                        for (int j = i; j < clients_count; j++) {
                            fds[j] = fds[j + 1];
                        }
                        printf("\n#--Disconnect client %d--#\n", clients_count);
                        clients_count -= 1;
                    }
                    else if (was_read == -1) {
                        perror("Error read() from socket");
                        exit(8);
                    }

                    for (ssize_t i = 0; i < was_read; i++) {
                        buf[i] = toupper(buf[i]);
                    }
                    ssize_t written = write(fileno(stdout), buf, was_read);
                    if (written == -1) {
                        perror("Error write()");
                        exit(9);
                    }
                }
            }
        }

    }

    printf("\n#--Turn off server--#\n");
    printf("\n#--Total number of clients processed: %d--#\n", total_clients_processed);
    close(socket_fd);
    free(fds);
    return 0;
}

