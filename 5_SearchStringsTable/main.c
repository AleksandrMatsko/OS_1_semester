#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

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
            close(fd);
            return NULL;
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

int printLineOnNum(TLineTable *table, off_t line_num, const int fd) {
    if (line_num > table->size || line_num < 1) {
        close(fd);
        perror("ERROR: line number out of range");
        destroyTable(table);
        return 5;
    }
    off_t length = table->table_data[line_num - 1].length;

    char buffer[BUFSIZ];
    off_t res = lseek(fd, table->table_data[line_num - 1].offset, SEEK_SET);
    if (res == -1) {
        perror("ERROR in lseek()");
        destroyTable(table);
        close(fd);
        return 6;
    }
    off_t already_read = 0;
    while (already_read < length) {
        ssize_t was_read = read(fd, buffer, BUFSIZ);
        if (was_read == -1) {
            perror("ERROR while reading file");
            destroyTable(table);
            close(fd);
            return 7;
        }
        already_read += was_read;
        if (already_read >= length) {
            was_read = length - (already_read - was_read);
        }
        ssize_t was_write = write(1, buffer, was_read);
        if (was_write == -1) {
            perror("ERROR while writing data");
            destroyTable(table);
            close(fd);
            return 8;
        }
    }
    return 0;
}

off_t GetLineNumber() {
    off_t line_number = -1;
    while (1) {
        printf("Enter number of line: ");
        int ret = scanf("%lld", &line_number);
        int res = fflush(stdin);
        if (res == EOF) {
            perror("ERROR while using fflush()");
            exit(9);
        }
        if (ret != 1 && !feof(stdin)) {
            printf("wrong input\n");
        }
        else if (feof(stdin)) {
            fprintf(stderr, "\nERROR: EOF in stdin\n");
            exit(10);
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
        return 2;
    }
    const int file_descriptor = open(argv[1], O_RDONLY);
    if (file_descriptor == -1) {
        perror("ERROR: can't open file");
        return 3;
    }
    TLineTable *table = makeTable(file_descriptor);
    if (table == NULL) {
        return 4;
    }
    printf("Number of lines in file: %lld\n", table->size);
    off_t line_number = GetLineNumber();
    while (line_number) {
        if (line_number > table->size || line_number < 0) {
            printf("line number out of range\n");
        }
        else {
            int res = printLineOnNum(table, line_number, file_descriptor);
            if (res != 0) {
                return res;
            }
        }
        line_number = GetLineNumber();
    }
    close(file_descriptor);
    destroyTable(table);
    return 0;
}
