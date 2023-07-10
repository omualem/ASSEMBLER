#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "globals.h"


int isRegisterLabel(const char* label) {
    /*Returns a number between 0 and 7 according to the
 * register in the label, otherwise none of them will return 8*/
    if(label==NULL)
        return non_reg;
    if(strcmp(label,"r0")==0)
        return r0;
    if(strcmp(label,"r1")==0)
        return r1;
    if(strcmp(label,"r2")==0)
        return r2;
    if(strcmp(label,"r3")==0)
        return r3;
    if(strcmp(label,"r4")==0)
        return r4;
    if(strcmp(label,"r5")==0)
        return r5;
    if(strcmp(label,"r6")==0)
        return r6;
    if(strcmp(label,"r7")==0)
        return r7;
    else
        return non_reg;
}

int isInstructionLabel(const char* label) {
    /*Returns a number between 0 and 15 according to the
     * opcode in the label, otherwise none of them will return 16*/
    if (label == NULL)
        return non_op;
    if (strcmp(label, "mov") == 0)
        return mov;
    if (strcmp(label, "cmp") == 0)
        return cmp;
    if (strcmp(label, "add") == 0)
        return add;
    if (strcmp(label, "sub") == 0)
        return sub;
    if (strcmp(label, "not") == 0)
        return not;
    if (strcmp(label, "clr") == 0)
        return clr;
    if (strcmp(label, "lea") == 0)
        return lea;
    if (strcmp(label, "inc") == 0)
        return inc;
    if (strcmp(label, "dec") == 0)
        return dec;
    if (strcmp(label, "jmp") == 0)
        return jmp;
    if (strcmp(label, "bne") == 0)
        return bne;
    if (strcmp(label, "red") == 0)
        return red;
    if (strcmp(label, "prn") == 0)
        return prn;
    if (strcmp(label, "jsr") == 0)
        return jsr;
    if (strcmp(label, "rts") == 0)
        return rts;
    if (strcmp(label, "stop") == 0)
        return stop;
    return non_op;
}

int isDirectiveLabel(const char* label) {
    /*Returns a number between 0 and 3 according to the directive
     * in the label, otherwise none of them will return 4*/
    if(label==NULL)
        return non_dir;
    if(strcmp(label,".data")==0)
        return DATA;
    if(strcmp(label,".string")==0)
        return STRING;
    if(strcmp(label,".entry")==0)
        return ENTRY;
    if(strcmp(label,".extern")==0)
        return EXTERN;
    else
        return non_dir;
}

int skipWhiteChars(const char* line,int* index){
    /*Skips white characters and returns the number of times it was skipped*/
    int whiteCounts = 0;
    while ((line[(*index)]==SPACE_BAR || line[(*index)]==TAB) && line[(*index)]!=END_OF_LINE){
        (*index)++;
        whiteCounts++;
    }
    return whiteCounts;
}

int findIndexFromLine(const char* line,char ch){
    /*Returns the index at which the character is in the line*/
    int index = 0;
    while (line[index]!=ch) {
        if (line[index] == END_OF_LINE)
            return 0;
        index++;
    }
    return index;
}

int fileExists(const char* filename) {
    /*Gets a file name and checks if it exists*/
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return TRUE;
    }
    return FALSE;
}

char* setOutputFile(const char* file,char* ext){
    /*Gets a name and a suffix and creates a new string of the name with the suffix.
     * For example, the name example and the suffix .am will be returned example.am)*/

    const char* fileName = file;
    const char* extension = ext;
    unsigned int fileNameLen = strlen(fileName);
    unsigned int extensionLen = strlen(extension);
    char* fileWithExtension = (char*) malloc(fileNameLen + extensionLen + 1);

    MALLOC_CHECK(fileWithExtension)

    strcpy(fileWithExtension, fileName);
    strcat(fileWithExtension, extension);

    return fileWithExtension;
}

char base64DecodeChar(char c) {
    /*Gets a number that represents a character and returns the number in base64*/
    if (c >= 0 && c <= 25)
        return c + 'A';
    else if (c >= 26 && c <= 51)
        return c - 26 + 'a';
    else if (c >= 52 && c <= 61)
        return c - 52 + '0';
    else if (c == 62)
        return '+';
    else if (c == 63)
        return '/';
    else
        return '\0';
}

int binaryToDecimal(const int binArray[]) {
    /*Receives an array representing a binary number and converts
     * its 6 cells (depending on what was sent) to the
     * numerical representation of the binary*/
    int result = 0;
    int i;
    for (i = 0; i < 6; i++) {
        result = (result << 1) | binArray[i];
    }
    return result;
}
void printBase64ToFile(char c1,char c2, FILE* obFile){
    /*Prints to a file 2 characters side by side and then drops a line*/
    fprintf(obFile,"%c",c1);
    fprintf(obFile,"%c",c2);
    fprintf(obFile,"\n");
}

void convertToCharsBase64(int binCode[],FILE* obFile) {
    /*Arrays that represent 2 parts of a 12-bit binary number*/
    int firstBinArr[6] = {0};
    int secondBinArr[6] = {0};

    int index = 0, i;
    char firstChar, secondChar,firstBinToDecimal,secondBinToDecimal;

    /*Divides 12-bit binary number into 2 numbers of a 6-bit*/
    for (i = 0; i < 6; i++) {
        firstBinArr[index] = binCode[11 - i];
        secondBinArr[index] = binCode[5 - i];
        index++;
    }

    /*Converts the binary numbers to decimal numbers*/
    firstBinToDecimal = binaryToDecimal(firstBinArr);
    secondBinToDecimal = binaryToDecimal(secondBinArr);

    /*Converts the decimal numbers to base64 characters*/
    firstChar = base64DecodeChar(firstBinToDecimal);
    secondChar = base64DecodeChar(secondBinToDecimal);

    /*Prints the received characters to the object file*/
    printBase64ToFile(firstChar,secondChar,obFile);
}

void decimalToBinary(int arr[],int number,int numberOfBits) {
    /*covert the number in a given number of bits, and enter
     * him into the binCode array*/
    int i;
    int mask = 1 << (numberOfBits - 1);

    for (i = numberOfBits - 1; i >= 0; i--) {
        arr[i] = (number & mask) ? 1 : 0;
        mask >>= 1;
    }
}

void charToBinary(char ch, int charArr[]) {
    /*Converts a character's ascii code to binary and puts it into an array*/
    int i;
    for (i = 0; i < 8; i++)
        charArr[i] = (ch >> i) & 1;
}

void resetInsArr(int sourceAdrArr[],int destAdrArr[],int opcodeArr[],int areArr[],int numInsArr[],int srcReg[],int desReg[]){
    /*reset every Instruction array  to 0*/
    memset(sourceAdrArr, 0, 3*sizeof(int));
    memset(destAdrArr, 0, 3*sizeof(int));
    memset(opcodeArr, 0, 4*sizeof(int));
    memset(areArr, 0, 2*sizeof(int));
    memset(numInsArr, 0, 10*sizeof(int));
    memset(srcReg, 0, 5*sizeof(int));
    memset(desReg, 0, 5*sizeof(int));
}

