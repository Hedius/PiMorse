/*
 *project morseReceive: list.h
 *this list is thread-safe!
 *license: GPLv3
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifndef INC_LIST_H
#define INC_LIST_H

struct list_elem {
    struct list_elem * next;
    struct list_elem * prev;

    int value; // 0 = Low, 1 = High
    int length; // for how long the value has been read

    char morseValue; // "-" "." " " "/"
};
typedef struct list_elem LIST_ELEM;

struct list_header {
    int len;
    LIST_ELEM * first, * last;

    int dit; // length of dit

    int status; // signal for decodeThread to terminate ( 1 = active, 0 = terminate)
    pthread_mutex_t mutex; // mutex for mutal exclusion of threads
};
typedef struct list_header LIST_HEADER;

extern LIST_HEADER * createList(void);

extern LIST_ELEM * insertFirst(LIST_HEADER*, int, int);
extern LIST_ELEM * insertLast(LIST_HEADER*, int, int);

extern void freeELEM(LIST_ELEM*);
extern LIST_ELEM * removeFirst(LIST_HEADER * header);
extern void deleteFirst(LIST_HEADER*);
extern LIST_ELEM * removeLast(LIST_HEADER * header);
extern void deleteLast(LIST_HEADER*);

extern void printList(LIST_HEADER*,FILE*);
extern void printListReverse(LIST_HEADER*, FILE*);

extern void rmList(LIST_HEADER*);

#endif //INCLIST_H
