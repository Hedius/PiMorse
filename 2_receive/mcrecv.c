/*
*file : mcrecv.c
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "libMorseReceive.h"


void printHelp(){
    printf("Usage: mcrecv [OPTION]\n");
    printf("Receive and decode morse code. Signals are received by a MCP3008 ADC.\n");
    printf("The program waits for the start of the transmission and stops receiving after 10 seconds of silence.\n");
    printf("\tvalid options are:\n");
    printf("\t\t-h\tdisplay this help and exit\n");
    printf("\t\t-t TRIGGER\t ADC trigger level (above = 0) (below = 1) (default = 600) (range: 0 - 1023)\n");
}


int main(int argc, char *argv[]){
    initADC();

    int size, dit;
    int * signal;
    char * morse, * str;

    int trigger = 600;

    int opt;
    while((opt = getopt(argc, argv, "h::t:")) != -1){
        switch(opt){
            case 'h': printHelp(); return 0;
            case 't': trigger = atoi(optarg); break;
        }
    }

    // Read Signal
    signal = readSignal(trigger, &size);
    if(!signal) exit(1);

    // Calc dit
    dit = determineDit(signal, size);


    // Signal to morse
    morse = convertRawtoMorse(signal, size, dit);
    if(!morse) exit(1);
    printf("Morse: %s \n", morse);


    // morse to ascii
    str = convertMorsetoASCII(morse);
    if(!str) exit(1);
    printf("String: %s \n", str);

    free(signal);
    free(morse);
    free(str);
    return 0;
}

