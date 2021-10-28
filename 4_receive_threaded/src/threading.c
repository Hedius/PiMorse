/*
 *project morseReceive: threading.h
 */
#include "threading.h"

/*
 * Function: receiveThreadFunc
 * main function for receiveThread
 * reads morse code until there is no signal for 49 dits
 * adds the read data to a given list
 * arguments: void pointer to RECEIVE_ARGS
 * returns: (void*)0
 */
void * receiveThreadFunc(void * arguments){
    RECEIVE_ARGS *args = (RECEIVE_ARGS*)arguments;
    
    readSignal(args->header, args->trigger);
    args->header->status = 0; // signal for converter that receiver is done
    pthread_exit((void *)0);
}

/*
 * Function: convertThreadFunc
 * main function for convertThread
 * converts new values in the given list to ASCII and prints the result
 * arguments: void pointer to CONVERT_ARGS
 * returns: (void*)0
 */
void * convertThreadFunc(void * arguments){
    CONVERT_ARGS *args = (CONVERT_ARGS*)arguments;

    while(args->header->len <=1) // we need at least 2 values
        delay(100);

    convertRawtoASCII(args->header);

    pthread_exit((void *)0);
}


/*
 * Function: startThreads
 * starts the threads
 * receiveArgs: RECEIVE_ARGS
 * convertArgs: CONVERT_ARGS
 * returns: (void*)0
 */
void startThreads(RECEIVE_ARGS * receiveArgs, CONVERT_ARGS * convertArgs){
    pthread_t receiveThread, convertThread;
    int status;
    
    // start producer
    status = pthread_create(&receiveThread, NULL, &receiveThreadFunc, (void*)receiveArgs);
    if(status != 0){
        fprintf(stderr, "Error while starting receiveThread!\n");
        exit(1);
    }

    // start consumer (really? does not remove any values form the list) ???
    status = pthread_create(&convertThread, NULL, &convertThreadFunc, (void*)convertArgs);
    if(status != 0){
        fprintf(stderr, "Error while starting convertThread!\n");
        exit(1);
    }

    // wait until producer terminates
    pthread_join(receiveThread, NULL);
    // wait until consumer terminates
    pthread_join(convertThread, NULL);
}
