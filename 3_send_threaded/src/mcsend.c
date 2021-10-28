/*
 *file : mcsend.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "list/list.h"
#include "threading.h"

int delayDit = 1200; // 1200 ms dit = 1 WpM
int pin = 1; // DEFAULT = GPIO18

void printHelp(){
    printf("Usage: mcsend [OPTION] [STRING]\n");
    printf("Send STRING (if given) or stdin as morse.\n");
    printf("\tvalid options are:\n");
    printf("\t\t-h\tdisplay this help and exit\n");
    printf("\t\t-s WPM\t speed in words per minute (1 WpM = 1200ms per dit) (Default = 8 WpM)\n");
    printf("\t\t-p PIN\tGPIO pin to use (Default = 1 (GPIO18))\n");
}

int main(int argc, char *argv[]){
    int opt;
    int speed = 8;
    LIST_HEADER * queue;
    READ_ARGS readArgs;
    SEND_ARGS sendArgs;

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

    queue = createList();
    if(!queue){ return -1; }

    readArgs.argc = argc;
    readArgs.argcount = argcount;
    readArgs.argv = argv;
    readArgs.header = queue;

    sendArgs.pin = pin;
    sendArgs.delayDit = delayDit;
    sendArgs.header = queue;

    startThreads(&readArgs, &sendArgs);

    printf("Done!\n");
    
    rmList(queue);
    return 0;
}
