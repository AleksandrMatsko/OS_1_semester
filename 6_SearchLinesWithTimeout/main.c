#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

typedef struct TableCell {
    off_t length;
    off_t offset;
} TCell;

typedef struct LineTable {
    off_t size;
    struct TableCell *table_data;
} TLineTable;

TLineTable *makeTable(const int fd) {
    size_t max_table_size = 4;
    TCell *table_data = (TCell *)calloc(max_table_size, sizeof(TCell));
    if (table_data == NULL) {
        perror("ERROR: calloc returned NULL");
        exit(1);
    }
    char buffer[BUFSIZ] = "";
    off_t line_number = 0;
    table_data[0].length = 0;
    table_data[0].offset = 0;
    while (1) {
        ssize_t was_read = read(fd, buffer, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("ERROR while reading file");
            exit(4);
        }

        for (int i = 0; i < was_read; i++) {
            if (buffer[i] == '\n') {
                if (max_table_size - 1 == line_number) {
                    max_table_size *= 2;
                    table_data = realloc(table_data, max_table_size * sizeof(TCell));
                    if (table_data == NULL) {
                        perror("ERROR: realloc returned NULL");
                        exit(1);
                    }
                }
                table_data[line_number].length += 1;
                table_data[line_number + 1].offset = table_data[line_number].length + table_data[line_number].offset;
                table_data[line_number + 1].length = 0;
                line_number += 1;
            }
            else {
                table_data[line_number].length += 1;
            }

        }
    }
    TLineTable *table = (TLineTable *)calloc(1, sizeof(TLineTable));
    if (table == NULL) {
        perror("ERROR: calloc returned NULL");
        exit(1);
    }
    table->table_data = table_data;
    table->size = line_number;
    return table;
}

void destroyTable(TLineTable *table) {
    free(table->table_data);
    free(table);
}

void printLineOnNum(TLineTable *table, off_t line_num, const int fd) {
    if (line_num > table->size || line_num < 1) {
        perror("ERROR: line number out of range");
        destroyTable(table);
        exit(5);
    }
    off_t length = table->table_data[line_num - 1].length;

    char buffer[BUFSIZ];
    off_t res = lseek(fd, table->table_data[line_num - 1].offset, SEEK_SET);
    if (res == -1) {
        perror("ERROR in lseek()");
        exit(6);
    }
    off_t already_read = 0;
    while (already_read < length) {
        ssize_t was_read = read(fd, buffer, BUFSIZ);
        if (was_read == -1) {
            perror("ERROR while reading file");
            exit(7);
        }
        already_read += was_read;
        if (already_read >= length) {
            was_read = length - (already_read - was_read);
        }
        ssize_t was_write = write(1, buffer, was_read);
        if (was_write == -1) {
            perror("ERROR while writing data");
            exit(8);
        }
    }
}

void printAllLines(const int fd) {
    off_t res = lseek(fd, 0, SEEK_SET);
    if (res == -1) {
        perror("ERROR in lseek()");
        exit(9);
    }
    char buffer[BUFSIZ] = "";
    while (1) {
        ssize_t was_read = read(fd, buffer, BUFSIZ);
        if (was_read == 0) {
            break;
        }
        else if (was_read == -1) {
            perror("ERROR while reading file");
            exit(10);
        }
        ssize_t was_write = write(1, buffer, was_read);
        if (was_write == -1) {
            perror("ERROR while writing data");
            exit(11);
        }
    }
}

off_t GetLineNumber(const int fd) {
    off_t line_number = -1;

    struct pollfd fds;
    fds.fd = fileno(stdin);
    fds.events = POLLIN;

    while (1) {
        printf("You have 5 seconds to enter number of line: \n");

        int count = poll(&fds, 1, 5000);
        if (count == -1) {
            perror("Error in poll()");
        }
        else if (count == 0) {
            printAllLines(fd);
            exit(0);
        }

        int ret = scanf("%lld", &line_number);
        int res = fflush(stdin);
        if (res == EOF) {
            perror("ERROR while using fflush()");
            exit(12);
        }
        if (ret != 1 && !feof(stdin)) {
            printf("wrong input\n");
        }
        else if (feof(stdin)) {
            fprintf(stderr, "\nERROR: EOF in stdin\n");
            exit(13);
        }
        else {
            break;
        }
    }
    return line_number;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: invalid number of arguments\n");
        exit(2);
    }
    const int file_descriptor = open(argv[1], O_RDONLY);
    if (file_descriptor == -1) {
        perror("ERROR: can't open file");
        exit(3);
    }
    TLineTable *table = makeTable(file_descriptor);
    printf("Number of lines in file: %lld\n", table->size);
    off_t line_number = GetLineNumber(file_descriptor);
    while (line_number) {
        if (line_number > table->size || line_number < 0) {
            printf("line number out of range\n");
        }
        else {
            printLineOnNum(table, line_number, file_descriptor);
        }
        line_number = GetLineNumber(file_descriptor);
    }
    close(file_descriptor);
    destroyTable(table);
    return 0;
}

