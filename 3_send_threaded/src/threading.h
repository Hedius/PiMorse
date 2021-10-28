/*
 *project morseSend: threading.h
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include "mcSendLib.h"
#include "list/list.h"

#ifndef INC_THREADING_H
#define INC_THREADING_H

// 2 Threads
// Producer: readThread: reads data from argv/stdin and adds the corresponding morse code to a list
// Consumer: sendThread: sends morse code from a list to GPIO (light signals)

// structure for parsing args to readThread
struct readArgs{
    int argc;
    int argcount;
    char **argv;
    LIST_HEADER * header;
};
typedef struct readArgs READ_ARGS;

// structure for parsing args to sendThread
struct sendArgs{
    int pin;
    int delayDit;
    LIST_HEADER * header;
};
typedef struct sendArgs SEND_ARGS;

extern void startThreads(READ_ARGS*, SEND_ARGS*);

#endif 
