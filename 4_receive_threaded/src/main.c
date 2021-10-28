/*
*project morseReceive: main.c
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "threading.h"
#include "list/list.h"


void printHelp(){
    printf("Usage: mcrecv [OPTION]\n");
    printf("Receive and decode morse code. Signals are received by a MCP3008 ADC.\n");
    printf("The program waits for the start of the transmission and stops receiving after 49 dits of silence.\n");
    printf("\tvalid options are:\n");
    printf("\t\t-h\tdisplay this help and exit\n");
    printf("\t\t-t TRIGGER\t ADC trigger level (above = 0) (below = 1) (default = 600) (range: 0 - 1023)\n");
}


int main(int argc, char *argv[]){
    initADC();

    int trigger = 600;
    LIST_HEADER * signal;
    RECEIVE_ARGS recArgs;
    CONVERT_ARGS conArgs;

    int opt;
    while((opt = getopt(argc, argv, "h::t:")) != -1){
        switch(opt){
            case 'h': printHelp(); return 0;
            case 't': trigger = atoi(optarg); break;
        }
    }

    signal = createList();
    if(!signal) { return -1; }

    recArgs.header = conArgs.header= signal;
    recArgs.trigger = trigger;

    startThreads(&recArgs, &conArgs);

    rmList(signal);
    return 0;
}

