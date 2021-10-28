/*
 *file : mcsend.c
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mcSendLib.h"


int delayDit = 1200; // 1200 ms dit = 1 WpM
int pin = 1; // DEFAULT = GPIO18

/*
 * Function: readString
 * reads a string from argv or stdin
 * argc: argcount from program call
 * argv: argvalues from program call
 * argcount: counter for used options
 * returns: buffer
 */
char * readString(int argc, char *argv[], int argcount){
    char * buffer = calloc(BUFFER_SIZE, sizeof(char));
    if(!buffer) return NULL;

    if(argc - 1 == argcount){
        printf("Pls enter the text:\n");
        fgets(buffer, BUFFER_SIZE, stdin);
    }
    else{
        for(int i = argcount + 1; i < argc; i++){
            strcat(buffer, argv[i]);
            strcat(buffer, " ");
        }
        buffer[strlen(buffer) - 1] = '\0';
    }
    return buffer;
}

void printHelp(){
    printf("Usage: mcsend [OPTION] [STRING]\n");
    printf("Send STRING (if given) or stdin as morse.\n");
    printf("\tvalid options are:\n");
    printf("\t\t-h\tdisplay this help and exit\n");
    printf("\t\t-s WPM\t speed in words per minute (1 WpM = 1200ms per dit) (Default = 8 WpM)\n");
    printf("\t\t-p PIN\tGPIO pin to use (Default = GPIO18)\n");
}

int main(int argc, char *argv[]){
    int opt;
    int speed = 8;
    char * buffer;
    char * morseString;


    // process argv
    int argcount = 0;
    while((opt = getopt(argc, argv, "h::s:p:")) != -1){
        switch(opt){
            case 'h': printHelp(); argcount++; return 0;
            case 's': speed = atoi(optarg); argcount+=2; break;
            case 'p': pin = atoi(optarg); argcount+=2; break;
        }
    }


    // setup pin
    wiringPiSetup();
    pinMode(pin, OUTPUT);

    if(speed == 0){
        printf("0 WPM is an invalid input!\n");
        return -1;
    }
    else{
        delayDit = 1200/speed;
    }


    buffer = readString(argc, argv, argcount);
    if(!buffer) return -1;

    morseString = convertASCIItoMorse(buffer);
    if(!morseString){ free(buffer); return -1; }
    printf("Read: %s\nMorse: %s\nSending: ", buffer, morseString);

    sendMorse(pin, delayDit, morseString);

    printf("Done!\n");
    
    free(buffer);
    free(morseString);
    return 0;
}
