#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

int main() {
    struct termios tty, saved_tty;
    int fd = open("/dev/tty", O_RDWR);
    if (fd == -1) {
        perror("open()");
        exit(1);
    }

    if (!isatty(fd)) {
        perror("fd not a terminal");
        exit(2);
    }

    tcgetattr(fd, &tty);
    saved_tty = tty;

    tty.c_lflag &= ~(ISIG | ICANON);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &tty);
    ssize_t written = write(fd, "\nAre ready for summer?\nEnter y/n: ", 34);
    if (written == -1) {
        perror("write()");
        tcsetattr(fd, TCSAFLUSH, &saved_tty);
        exit(3);
    }
    char symbol;
    ssize_t was_read = read(fd, &symbol, 1);
    if (was_read == -1) {
        perror("read()");
        tcsetattr(fd, TCSAFLUSH, &saved_tty);
        exit(4);
    }
    if (symbol == 'y') {
        written = write(fd, "\nNice\n", 6);
        if (written == -1) {
            perror("write()");
            tcsetattr(fd, TCSAFLUSH, &saved_tty);
            exit(5);
        }
    }
    else if (symbol == 'n') {
        written = write(fd, "\nIt's time to think\n", 20);
        if (written == -1) {
            perror("write()");
            tcsetattr(fd, TCSAFLUSH, &saved_tty);
            exit(6);
        }
    }
    else {
        written = write(fd, "\nYou entered wrong symbol\n", 26);
        if (written == -1) {
            perror("write()");
            tcsetattr(fd, TCSAFLUSH, &saved_tty);
            exit(7);
        }
    }
    tcsetattr(fd, TCSAFLUSH, &saved_tty);
    return 0;
}
