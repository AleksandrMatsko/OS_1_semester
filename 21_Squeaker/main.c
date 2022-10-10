#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

int squeak_counter = 0;
bool is_stop = false;

void sigCatch(int sig) {
    if (sig == SIGINT) {
        write(fileno(stdout), "\a\nsqueak\n", 9);
        squeak_counter += 1;
    }
    else if (sig == SIGQUIT) {
        is_stop = true;
    }
}


int main() {
    if (sigset(SIGINT, sigCatch) == SIG_ERR) {
        perror("Error in sigset for SIGINT");
        exit(1);
    }

    if (sigset(SIGQUIT, sigCatch) == SIG_ERR) {
        perror("Error in sigset for SIGQUIT");
        exit(2);
    }

    while (1) {
        pause();
        if (is_stop) {
            break;
        }
    }

    printf("\nsqueaked %d times\n", squeak_counter);
    return 0;
}
