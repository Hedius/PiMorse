/*
 *file mcSendLib.h
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef INC_MCSENDLIB_H
#define INC_MCSENDLIB_H

#define BUFFER_SIZE 1000

extern char* CHAR_TO_MORSE[128];

extern char * convertASCIItoMorse(char * asciiStr);
extern void sendMorse(int pin, int delayDit, char * morseString);

#endif
