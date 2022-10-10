#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct TList {
    char *string;
    struct TList *next;
} TList;

TList *createNode(char *s, int s_len) {
    TList *new_list = (TList *)malloc(sizeof(TList));
    if (new_list == NULL) {
        perror("Error: malloc returned NULL");
        exit(-1);
    }
    new_list->string = (char *) malloc(sizeof(char) * s_len);
    if (new_list->string == NULL && s_len != 0) {
        perror("Error: malloc returned NULL");
        exit(-1);
    }
    memcpy(new_list->string, s, s_len);
    new_list->next = NULL;
    return new_list;
}

void add(TList **list, char *s, int s_len) {
    if (*list == NULL) {
        *list = createNode(s, s_len);
        return;
    }
    TList *tmp = *list;
    while ((*list)->next != NULL) {
        *list = (*list)->next;
    }
    (*list)->next = createNode(s, s_len);
    *list = tmp;
}

void destroyList(TList *list) {
    while (list != NULL) {
        TList *tmp = list->next;
        free(list->string);
        free(list);
        list = tmp;
    }
}

void printList(TList *list) {
    while (list != NULL) {
        fputs(list->string, stdout);
        list = list->next;
    }
}

int main() {
    const size_t BUF_SIZE = 64;
    TList *list = NULL;
    char buf[BUF_SIZE];
    int mul = 1;
    int index = 0;
    char *string = (char *)malloc(sizeof(char) * BUF_SIZE * mul);
    while (1) {
        char *res = fgets(buf, BUF_SIZE, stdin);
        if (feof(stdin)) {
            fprintf(stderr, "ERROR: EOF registered before .\n");
            free(string);
            return -1;
        }
        if (res == NULL && !feof(stdin)) {
            perror("Error while reading data");
            free(string);
            return -1;
        }
        if (buf[0] == '.' && index == 0) {
            break;
        }
        int s_len = strlen(buf);
        if (index + s_len >= mul * BUF_SIZE - 1) {
            mul *= 2;
            string = realloc(string, mul * BUF_SIZE * sizeof(char));
            if (string == NULL) {
                perror("Error: realloc returned NULL");
                free(string);
                destroyList(list);
                return -2;
            }
        }
        memcpy(&string[index], buf, BUF_SIZE *  sizeof(char));
        index += s_len;
        if (strchr(buf, '\n') != NULL) {
            add(&list, string, strlen(string) + 1);
            index = 0;
            memset(string, 0, mul * BUF_SIZE * sizeof(char));
        }
    }
    free(string);
    printList(list);
    destroyList(list);
    return 0;
}
