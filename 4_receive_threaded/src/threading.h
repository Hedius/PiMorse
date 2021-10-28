/*
 *project morseReceive: threading.h
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include "morseReceive.h"
#include "list/list.h"

#ifndef INC_THREADING_H
#define INC_THREADING_H

// 2 Threads
// Producer: receiveThread: reads the raw signal by using a MCP3008
// Consumer: convertThread: converts the raw signal in the given list to ASCII code and prints the result to stdout

// structure for parsing args to receiveThread
struct receiveArgs{
    int trigger;
    LIST_HEADER * header;
};
typedef struct receiveArgs RECEIVE_ARGS;

// structure for parsing args to convertThread
struct convertArgs{
    LIST_HEADER * header;
};
typedef struct convertArgs CONVERT_ARGS;

extern void startThreads(RECEIVE_ARGS*, CONVERT_ARGS*);

#endif 
