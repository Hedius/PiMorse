/*
 *project morseSend: threading.h
 */
#include "threading.h"

/*
 * Function: readThreadFuc
 * main function for readThread
 * reads data from argv or from stdin in a loop until :q is typed
 * adds the read data to a given list
 * arguments: void pointer to READ_ARGS
 * returns: (void*)0
 */
void * readThreadFunc(void * arguments){
    char buffer[BUFFER_SIZE] = "";

    READ_ARGS *args = (READ_ARGS*)arguments;

    if(args->argc - 1 == args->argcount){
        // No args --- read text in loop
        printf("Pls enter a text or ':q' to exit:\n");
        while(1){
            printf("Text: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strlen(buffer) - 1] = '\0'; // remove \n

            if(strcmp(":q", buffer) == 0)
                break;

            if(strlen(buffer) == 0)
                continue;

            printf("Adding string \"%s\" to queue!\nMorse: ", buffer);
            convertASCIItoMorse(args->header, buffer);
            printf("\n");
        }
    }
    else{
        // process argv
        for(int i = args->argcount + 1; i < args->argc; i++){
            strcat(buffer, args->argv[i]);
            strcat(buffer, " ");
        }
        buffer[strlen(buffer) - 1] = '\0'; // remove last space

        printf("Read string \"%s\" from argv!\nMorse: ", buffer);
        convertASCIItoMorse(args->header, buffer);
        printf("\n");
    }

    args->header->status = 0; // signal for sendThread to terminate as soon as the list is empty
    pthread_exit((void *)0);
}

/*
 * Function: sendThreadFuc
 * main function for sendThread
 * sends morse from the given list for as long as the producer is active
 * arguments: void pointer to SEND_ARGS
 * returns: (void*)0
 */
void * sendThreadFunc(void * arguments){
    SEND_ARGS *args = (SEND_ARGS*)arguments;

    while(1){
        sendMorse(args->pin, args->delayDit, args->header);
        if(args->header->len == 0 && args->header->status == 0)
            pthread_exit((void *)0);
        delay(100);
    }

    pthread_exit((void *)0);
}



void startThreads(READ_ARGS * readArgs, SEND_ARGS * sendArgs){
    pthread_t readThread, sendThread;
    int status;
    
    // start producer
    status = pthread_create(&readThread, NULL, &readThreadFunc, (void*)readArgs);
    if(status != 0){
        fprintf(stderr, "Error while starting readThread!\n");
        exit(1);
    }

    // start consumer
    status = pthread_create(&sendThread, NULL, &sendThreadFunc, (void*)sendArgs);
    if(status != 0){
        fprintf(stderr, "Error while starting sendThread!\n");
        exit(1);
    }

    // wait until producer terminates
    pthread_join(readThread, NULL);

    printf("Please wait until the queue is empty!\n");

    // wait until consumer terminates
    pthread_join(sendThread, NULL);
}
