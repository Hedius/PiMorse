/*
*project morseReceive : morseReceive.h
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <wiringPi.h>
#include <mcp3004.h>

#include "list/list.h"
#include "morseToAsciiTable.h"

#ifndef INC_MCRECVLIB_H
#define INC_MCRECVLIB_H

#define MCP3008_PIN_BASE 100
#define MCP3008_SPI_CHAN 0

#define DELAY 1000 // µs check the ADC every ms


extern void initADC();
extern int readSignal(LIST_HEADER*, int);
extern void convertRawtoASCII(LIST_HEADER*);

#endif


