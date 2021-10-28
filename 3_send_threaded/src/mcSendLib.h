/*
 *project morseSend: mcSendLib.h
 */
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "list/list.h"

#ifndef INC_MCSENDLIB_H
#define INC_MCSENDLIB_H

#define BUFFER_SIZE 1000

extern char * CHAR_TO_MORSE[128];

extern int convertASCIItoMorse(LIST_HEADER*, char*);
extern int sendMorse(int, int, LIST_HEADER*);

#endif
