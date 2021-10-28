/*
*project morseReceive : morseReceive.c
*/
#include "morseReceive.h"

/*
* Function: initADC
* initializes the ADC MCP3004
*/
void initADC(){
    if(wiringPiSetup() != 0){
        fprintf(stderr, "Setup of wiringPi failed!");
        exit(1);
    }
    if (mcp3004Setup(MCP3008_PIN_BASE,MCP3008_SPI_CHAN) == 0){
        printf("Setup of MCP3004 failed!");
        exit(1);
    }
}

/*
* Function: readValue
* reads MCP3008_SPI_CHAN
* returns: value of channel (max 1024)
*/
int readValue(){
    return analogRead(MCP3008_PIN_BASE + MCP3008_SPI_CHAN);
}


// reading of signal
/*
* Function: waitForSignal 
* runs in a loop for as long as we do not receive a signal
* trigger: trigger level
* returns: value of channel (max 1024)
*/
void waitForSignal(int trigger){
    while(readValue() > trigger)
        usleep(500);
}

/*
* Function: readSignal
* waits for the start of the transmission and saves all signal values in a list
* header: LIST_HEADER
* trigger: trigger level
* returns: array holding the signal (1=Light On, 0 = Light Off)
*/
int readSignal(LIST_HEADER * header, int trigger){
    LIST_ELEM * new;
    int curVal; // current reading
    int length = 1; // how often did we read curVal?
    int lastVal = 1; // reading before = for detecting change
    int countZeros = 0; //stop recording if we read too many zeroes

    if(!header) return -1;

    // wait for the start of the signal
    waitForSignal(trigger);

    // read the signal
    while(1){
        // check value
        if(readValue() <= trigger){
            curVal = 1;
            countZeros = 0;
        }
        else
            curVal = 0;

        if(lastVal != curVal){
            new = insertLast(header, lastVal, length);
            if(!new){
                fprintf(stderr, "Error while adding value to list! Exit!\n");
                exit(1);
            }
            length = 1;
        }

        // add a / for breaks and end the thread after 49 dits of 0
        if(length >= (7 * header->dit) && curVal == 0){ // 10 * dit
            if(header->last->value == 1){
                new = insertLast(header, 0, length);
                if(!new){
                    fprintf(stderr, "Error while adding value to list! Exit!\n");
                    exit(1);
                }
            }
            if(countZeros >= 7)
                break;

            length = 0;
            countZeros++;
        }

        length++;
        lastVal = curVal;
        usleep(DELAY);
    }
    return 0;
}

/*
* Function: convertRawtoMorseChar
* converts a raw reading into a morse char
* elem: LIST_ELEM
* dit: length of dit
*/
void convertRawtoMorseChar(LIST_ELEM * elem, int dit){
    if(!elem) return;
    double ratio = elem->length / (double)dit;

    if(0.5 <= ratio && ratio <= 1.5){
        if(elem->value == 1)
            elem->morseValue = '.';
        else
            elem->morseValue = 0;
    }
    else if(2.0 <= ratio && ratio <= 4.0){
        if(elem->value == 1)
            elem->morseValue = '-';
        else
            elem->morseValue = ' ';
    }
    else if(5.5 <= ratio && ratio <= 8.5){
        if(elem->value == 0)
            elem->morseValue = '/';
    }
    else if(ratio > 8.5)
        elem->morseValue = 'n';
    else
        elem->morseValue = '?';
}

/*
 * Function: getASCIICode
 * takes a morse symbol and returns its ASCII value
 * symbol: char array holding morse symbol
 * returns: ASCII ID or -1 on no result
 */
int getASCIICode(char * symbol){
    for(int i = 0; i < sizeof(morseToAsciiLetters)/sizeof(morseToAsciiLetters[0]); i++){
        if(strcmp(morseToAsciiLetters[i], symbol) == 0){
            return 'a' + i;
        }
    }
    for(int i = 0; i < sizeof(morseToNumbersSymbols)/sizeof(morseToNumbersSymbols[0]); i++){
        if(strcmp(morseToNumbersSymbols[i], symbol) == 0){
            return ' ' + i;
        }
    }
    return -1;
}

/*
 * Function: convertMorseChartoASCIIChar
 * takes a morse symbol and returns its ASCII value
 * header: LIST_HEADER
 * begin: LIST_ELEM: beginn of char in list
 * end: LIST_ELEM: end of char in list
 * returns: ASCII ID or -1 if no result
 */
char convertMorseChartoASCIIChar(LIST_HEADER * header, LIST_ELEM * beginn, LIST_ELEM * end){
    if(!header || !beginn || !end) return -1;
    char buffer[10] = "";
    int len;
    LIST_ELEM * cur;

    if(header->first == beginn) // fix for first element in list
        cur = beginn;
    else
        cur = beginn->next;

    if(header->last == end) // fix for last element in list
       end = NULL; 

    do{ // combine list elements to string
        if(cur->morseValue == '.' || cur->morseValue == '-'){
            len = strlen(buffer); 
            buffer[len] = cur->morseValue;
            buffer[len + 1] = '\0';
        }
        cur = cur->next;
    }while(cur != end);

    return getASCIICode(buffer);
}

/*
* Function: convertRawtoASCII
* goes through the given list, converts raw data to ascii and prints the results to stdout
* signal: pointer to the received signal
* header: LIST_HEADER
*/
void convertRawtoASCII(LIST_HEADER * header){
    if(!header) return;
    
    double ratio;
    char asciiChar;

    LIST_ELEM * cur, * lastSpace;

    cur = lastSpace = header->first;
    header->dit = header->first->next->length; // 1 dit = space between 2 morse chars

    while(1){
        // process cur element
        convertRawtoMorseChar(cur, header->dit); // convert current raw value into morse char

        // = space = convert Morse to Ascii
        if(cur->morseValue == ' ' || cur->morseValue == '/' || cur->morseValue == 'n'){
            pthread_mutex_lock(&header->mutex);
            asciiChar = convertMorseChartoASCIIChar(header, lastSpace, cur);
            pthread_mutex_unlock(&header->mutex);

            if(asciiChar != -1)
                printf("%c", convertMorseChartoASCIIChar(header, lastSpace, cur));

            lastSpace = cur;
        }

        if(cur->morseValue == '/')
            printf(" ");
        if(cur->morseValue == 'n')
            printf("\n");
        fflush(stdout); // force print
        
        while(!cur->next){ // wait for a new value
            if(cur == header->last && header->status == 0) // terminate if we are done
                return;
            delay(100);
        }
        cur = cur->next;

        // calculate new dit
        ratio = cur->length / (double)header->dit;
        if(ratio >= 0.7 && ratio <= 1.3){
            header->dit = (header->dit + cur->length) / 2;
        }
    }
}
