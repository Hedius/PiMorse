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
 * Function: addChartoStr
 * appends a char to str and reallocates str to its new size
 * str: main string
 * newChar: char to add to newStr
 * returns: pointer to str
 */
char * addChartoStr(char * str, char newChar){
    if(!str || !newChar) exit(1);
    int len = strlen(str);
    
    str = realloc(str, sizeof(char) * (len + 2));
    if(!str) exit(1);

    str[len] = newChar;
    str[len + 1] = '\0';
    return str;
}


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


// Main operation

/*
 * Function: convertASCIItoMorse
 * converts an ASCII string to morse code
 * asciiStr: string to convert
 * returns: pointer to morse code
 */
char * convertASCIItoMorse(char * asciiStr){
    if(!asciiStr) return NULL;
    
    char * morseString = malloc(sizeof(char));
    if(!morseString)return NULL;
    *morseString = '\0';

    for(int i = 0; i < strlen(asciiStr); i++){
        if(asciiStr[i] == ' ')
            morseString = addStrtoStr(morseString, "/ ");
        else{
            morseString = addStrtoStr(morseString, CHAR_TO_MORSE[asciiStr[i]]);
            morseString = addChartoStr(morseString, ' ');
        }
    }
    
    return morseString;
}

/*
 * Function: sendMorse
 * sends a given morse string to a GPIO
 * pin: GPIO id
 * delayDit: length of dit in ms
 * morseString: string to send
 */
void sendMorse(int pin, int delayDit, char * morseString){
    if(!morseString) return;
    for(int i = 0; i < strlen(morseString); i++){
        if(morseString[i] == '.'){ // Dit
            sendSignal(pin, delayDit);
            delay(delayDit);
        }
        else if(morseString[i] == '-'){ // Dah
            sendSignal(pin, 3 * delayDit);
            delay(delayDit);
        }
        else if(morseString[i] == ' ')
            if(morseString[i - 1] == '/')
                delay(1 * delayDit); // after /
            else
                delay(2 * delayDit); // after . -
        else
            delay(3 * delayDit); // = / 7 dits between words
    }
}
