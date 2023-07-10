#include "lexer_utils.h"
#include "globals.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void addressingAnalyze(st_ptr st){
    int numOfOperands = st->numOfOperands;

    /*Adjusts the addressing method according to the type of operand*/
    if(numOfOperands == 1 || numOfOperands == 2){
        switch (st->destOpType) {
            case number:
                st->destAdrMethod=immediate;
                break;
            case label:
                st->destAdrMethod=direct;
                break;
            case reg:
                st->destAdrMethod=reg_direct;
                break;
        }
        if(numOfOperands == 2){
            switch (st->sourceOpType) {
                case number:
                    st->sourceAdrMethod=immediate;
                    break;
                case label:
                    st->sourceAdrMethod=direct;
                    break;
                case reg:
                    st->sourceAdrMethod=reg_direct;
                    break;
            }
        }
    }
}

int findFirstNumber(const char* line,int labelFlag,st_ptr st){
    int index=0;
    /*in case theres a label with numbers in it*/
    if(labelFlag==TRUE)
        index+= (int )strlen(st->label)+1;
    while (line[index]  != '-' && line[index] != '+' && !(line[index] >= '0' && line[index] <= '9')) {
        if (line[index] == END_OF_LINE)
            return 0;
        index++;
    }
    return index;
}

int isValidOpLabel(const char* label){
    int count=0;

    /*If the label length is greater than the allowed length*/
    if (strlen(label) > MAX_LABEL_SIZE)
        return FALSE;

    /*If the label does not start with a letterIf the label does not start with a letter*/
    if(label[0]<'A' || label[0]>'z')
        return FALSE;

    /*If the label is not the name of an instruction/directive/register*/
    if (isInstructionLabel(label) != non_op || isDirectiveLabel(label) != non_dir || isRegisterLabel(label) != non_reg)
        return FALSE;

    /*If there is an invalid character in the label*/
    while (count!= strlen(label)){
        if(!isalpha(label[count]) && !isdigit(label[count]))
            return FALSE;
        count++;
    }

    return TRUE;
}

int isValidOpNumber(char* number){
    int arrLength = strlen(number),numArrIndex=1,numAfterConv=0;
    char numArr[MAX_LENGTH_LINE];
    strcpy(numArr,number);

    /*If the string does not start with a plus/minus/number*/
    if(numArr[0]!=MINUS && numArr[0]!=PLUS && !isdigit(numArr[0]))
        return FALSE;

    /*If there is a character that is not a number in the string*/
    while (numArrIndex!= arrLength){
        if(!isdigit(numArr[numArrIndex]))
            return FALSE;
        numArrIndex++;
    }

    /*The number is correct - converts it to an integer*/
    numAfterConv = strtol(number,NULL,10);

    /*Is the number within the range*/
    if(numAfterConv > MAX_VALID_INS_NUMBER || numAfterConv < MIN_VALID_INS_NUMBER)
        return FALSE;

    return TRUE;
}

int getNumOfOperands(int opcode){
    int numOfOperands=0;

    /*First group - receives 2 operands*/
    if (opcode == mov || opcode == cmp || opcode == add || opcode == sub || opcode == lea) {
        numOfOperands = 2;
    }

        /*Second group - receives 1 operand*/
    else if (opcode == not || opcode == clr || opcode == inc || opcode == dec || opcode == jmp ||
             opcode == bne || opcode == red || opcode == prn || opcode == jsr) {
        numOfOperands = 1;
    }

        /*Third group - does not receive operands*/
    else if (opcode == rts || opcode == stop) {
        numOfOperands = 0;
    }

    return numOfOperands;
}

int isValidLabel(const char *label, symbol_ptr symbol_head,const char *filename,int currentLine,int* errorFlag,int labelType) {
    symbol_ptr temp = symbol_head;
    int count=0;
    if(label==NULL){
        printf("Error: label has been not defined in line %d in %s\n",currentLine,filename);
        SET_ERROR
    }

    if (strlen(label) > MAX_LABEL_SIZE) {
        printf("Error: The label length %s in file %s in line %d is longer than the allowed length\n", label,filename, currentLine);
        SET_ERROR
    }

    if(label[0]<'A' || label[0]>'z'){
        printf("Error: The label %s in file %s in line %d isn't starting with an alphabetic letter\n", label,filename, currentLine);
        SET_ERROR
    }

    while (count!= strlen(label)){
        if(!isalpha(label[count]) && !isdigit(label[count])){
            printf("Error: The label %s in file %s in line %d has an illegal char\n", label,filename, currentLine);
            SET_ERROR
        }
        count++;
    }
    if (isInstructionLabel(label) != non_op || isDirectiveLabel(label) != non_dir || isRegisterLabel(label) != non_reg) {
        printf("Error: The label %s in file %s in line %d cannot be in the name of directive or an instruction \n", label,filename, currentLine);
        SET_ERROR
    }
    if(temp!=NULL){
        while (temp!=NULL){
            if(strcmp(temp->name,label)==0 && temp->type==labelType){
                printf("Error: The label %s in file %s in line %d is already defined \n", label,filename, currentLine);
                SET_ERROR
            }
            if((strcmp(temp->name,label)==0 && temp->type==external && labelType==entry)  ||
               (strcmp(temp->name,label)==0 && temp->type==entry && labelType==external)){
                printf("Error: The label %s in file %s in line %d is already defined as external\\internal\n", label,filename, currentLine);
                SET_ERROR
            }
            temp=temp->next;
        }
    }
    return TRUE;
}
