/*
 *project morseSend: mcSendLib.c
 */
#include "mcSendLib.h"

char * CHAR_TO_MORSE[128] = {
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "-.-.--", ".-..-.", "", "", "", "", ".----.",
    "-.--.", "-.--.-", "", "", "--..--", "-....-", ".-.-.-", "-..-.",
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
    "---..", "----.", "---...", "", "", "-...-", "", "..--..",
    ".--.-.", ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
    "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
    ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
    "-..-", "-.--", "--..", "", "", "", "", "..--.-",
    "", ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
    "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
    ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
    "-..-", "-.--", "--..", "", "", "", "", ""
};

// Local help functions

/*
 * Function: sendSignal
 * sends a high for del
 * pin: GPIO id
 * del: length of high-state
 */
void sendSignal(int pin, int del){
    digitalWrite(pin, HIGH);
    delay(del);
    digitalWrite(pin, LOW);
}

/*
 * Function: addStrtoList
 * adds a new entry to the given list
 * header: LIST_HEADER
 * curChar: ASCII character
 * curMorse: morse string
 * returns: pointer to new LIST_ELEM
 */
LIST_ELEM * addStrtoList(LIST_HEADER * header, char curChar, char* curMorse){
    LIST_ELEM * new;
    if(!header || !curMorse) return NULL;

    char * morse = calloc(strlen(curMorse) + 1, sizeof(char));
    if(!morse) return NULL;
    strcpy(morse, curMorse);

    new = insertLast(header, curChar, morse);
    if(!new){
        fprintf(stderr, "Error while adding character to list!\n");
        exit(1);
    } 
    printf("%s", curMorse);
    return new;
}


// Main operation

/*
 * Function: convertASCIItoMorse
 * converts an ASCII string to morse code and adds it to given list
 * header: LIST_HEADER
 * asciiStr: string to convert
 * returns: 0 on success, -1 on failure
 */
int convertASCIItoMorse(LIST_HEADER * header, char * asciiStr){
    if(!asciiStr) return -1;
    
    for(int i = 0; i <= strlen(asciiStr); i++){
        if(asciiStr[i] == ' ' || i == strlen(asciiStr)) // space between word + and at end of str
            addStrtoList(header, ' ', "/");
        else{ // ignore spaces
            if(asciiStr[i] > 127) // invalid letters ä,ö,ß,...
                continue;
            addStrtoList(header, asciiStr[i], CHAR_TO_MORSE[asciiStr[i]]);
        }
        addStrtoList(header, ' ', " "); // space between characters
    }
    return 0;
}


/*
 * Function: sendMorse
 * sends data from the given list until it is empty
 * pin: ID of GPIO pin
 * int: length of a dit
 * header: LIST_HEADER
 * returns: 0 on success, -1 on failure
 */
int sendMorse(int pin, int delayDit, LIST_HEADER * header){
    if(!header) return -1;
    LIST_ELEM * cur;

    while(header->len > 0){
        cur = removeFirst(header);
        if(!cur->morse) exit(1); // critical error

        for(int i = 0; i < strlen(cur->morse); i ++){
            if(cur->morse[i] == '.'){ // Dit
                sendSignal(pin, delayDit);
                delay(delayDit);
            }
            else if(cur->morse[i] == '-'){ // Dah
                sendSignal(pin, 3 * delayDit);
                delay(delayDit);
            }
            else if(cur->morse[i] == ' ' || cur->morse[i] == '/')
                delay(2 * delayDit); // after . /
        }
        freeELEM(cur);
    }
    return 0;
}
