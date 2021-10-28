/*
*file : libMorseReceive.c
*/
#include "libMorseReceive.h"
#include "MorseToAsciiTable.h"

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
* waits for the start of the transmission and saves all signal values in a dynamic array
* trigger: trigger level
* size: pointer to size of array
* returns: array holding the signal (1=Light On, 0 = Light Off)
*/
int * readSignal(int trigger, int * size){
    int curVal;
    int countZeros = 0;
    int signalSize = 0;

    // array holding all values
    int * signal = malloc(sizeof(int));
    if(!signal) return NULL;

    // wait for the start of the signal
    waitForSignal(trigger);

    // read the signal
    while(1){
        curVal = readValue();

        // resize array
        signalSize++;
        signal = realloc(signal, signalSize * sizeof(int));
        if(!signal) return NULL;

        if(curVal <= trigger){ // Light on
            signal[signalSize - 1] = 1;
            countZeros = 0;  // count number of read zeros - stop the reading after 10 seconds
        }
        else{ // Light off
            signal[signalSize - 1] = 0;
            countZeros++;
        }

        if(countZeros >= 6 * 1000000 / DELAY) // 10s lang 0 = end
            break;

        usleep(DELAY);
    }
    *size = signalSize;
    return signal;
}

/*
* Function: determineDit
* dit = minimum sequence of 1 in signal
* signal: pointer to the received signal
* size: size of array
* returns: dit
*/
int determineDit(int * signal, int size){
    int curLength = 0;
    int dit = size;
    for(int i = 0; i < size; i++){
        if(signal[i] == 1){
            curLength++;
        }
        else if(signal[i] == 0 && curLength != 0){
            if(curLength < dit)
                dit = curLength;
            curLength = 0;
        }
    }
    return dit;
}

// Transform received signal to morse code

/*
* Function: addStrtoStr
* appends newStr to str and reallocates str to its new size
* str: main string
* newStr: string to add to newStr
* returns: pointer to str
*/
char * addStrtoStr(char * str, char * newStr){
    if(!str || !newStr) exit(1);
    str = realloc(str, sizeof(char) * (strlen(str) + strlen(newStr) + 1));
    if(!str) exit(1);
    strcat(str, newStr);
    return str;
}

/*
* Function: convertRawtoMorse
* converts the raw signal to morse code by dividing each sequence by dit.
* signal: array holding the raw signal
* size: size of signal
* dit: dit
* returns: pointer to morse code (string)
*/
char *  convertRawtoMorse(int * signal, int size, int dit){
    int curLen = 1;
    double ratio;

    // array holding all values
    char * morse = malloc(sizeof(char));
    if(!morse) return NULL;
    *morse = '\0';

    for(int i = 0; i < (size - 1); i ++){
        curLen++;

        if(signal[i] != signal[i + 1]){ // Flanke
            ratio = curLen / (double)dit;
            if(signal[i] == 1){
                if(0.5 <= ratio && ratio <= 1.5) { // DIT
                    morse = addStrtoStr(morse, ".");
                }
                else if(2.0 <= ratio && ratio <= 4.0) { // DAH = 3 Dit
                    morse = addStrtoStr(morse, "-");
                }
                else {
                    morse = addStrtoStr(morse, "?"); // error
                }
            }
            else{
                if(2.0 <= ratio && ratio <= 4.0) { // Pause zwischen Buchstaben = 3 Dit
                    morse = addStrtoStr(morse, " ");
                }
                else if(5.5 <= ratio && ratio <= 8.5) {// Pause zwischen Wort = 7 Dit
                    morse = addStrtoStr(morse, " / ");
                }
                else if(ratio <= 0.5 && ratio >= 8.5) {
                    morse = addStrtoStr(morse, "?"); // error
                }
            }
            curLen = 0;
        }
    }
    return morse;
}

// Morse to Ascii
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
 * Function: addChartoStr
 * appends a char to str and reallocates str to its new size 
 * str: main string
 * newSymbol: morse symbol
 * returns: pointer to str
 */
char * addChartoStr(char * str, char * newSymbol){
    if(!str || !newSymbol) exit(1);

    int len = strlen(str);
    
    char newChar = getASCIICode(newSymbol);
    if(newChar == -1) return str;

    str = realloc(str, sizeof(char) * (len + 2));
    if(!str) exit(1);

    str[len] = (char)newChar;
    str[len + 1] = '\0';
    return str;
}

/*
 * Function: convertMorsetoASCII
 * takes a morse code string and converts it to a ASCII string
 * morse: morse code string
 * returns: ASCII string
 */
char *  convertMorsetoASCII(char * morse){
    int curLen = 0;
    char buffer[10] = "";

    // array holding all values
    char * str = malloc(sizeof(char));
    if(!str) return NULL;
    *str = '\0';

    for(int i = 0; i < strlen(morse); i++){
        if(morse[i] == ' '){
            str = addChartoStr(str, buffer);
            curLen = 0;
        }
        else if(morse[i] == '/'){
            str = addChartoStr(str, " ");
            i++;
        }
        else{
            buffer[curLen] = morse[i];
            buffer[curLen + 1] = '\0';
            curLen++;
        }
    }
    str = addChartoStr(str, buffer); // Last char
    return str;
}
