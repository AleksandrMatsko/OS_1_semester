#include <stdio.h>
extern char **environ;

/* program to be exec(2)'ed */

int main(int argc, char *argv[ ]) {
    char **p;
    int n;

    printf("My input parameters(argv) are:\n");
    for (n = 0; n < argc; n++) {
        printf(" %2d: '%s'\n", n, argv[n]);
    }

    printf("\nMy environment variables are:\n");
    for (p = environ; *p != 0; p++) {
        printf(" %s\n", *p);
    }
}
