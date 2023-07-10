#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "lexer_utils.h"
#include "globals.h"
#include "preprocess.h"
#include "utils.h"

/**
 * Gets the type of the operand (number/label/register).
 *
 * @param operand The operand to analyze.
 * @param st The st node of the line.
 * @param op_method The operand  method (source operand or destination operand).
 * @param currentLine The current line number.
 * @param filename The name of the source file.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 * */
static int getOperandType(char* operand,st_ptr st,int op_method,int currentLine,const char* filename,int* errorFlag);


/**
 * Analyzes the operands in the given line.
 *
 * @param line The given line.
 * @param currentLine The current line number (for error message).
 * @param filename The name of the source file (for error message).
 * @param st The st node of the line.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
static int operandsAnalyze(char* line,int currentLine,const char* filename,st_ptr st,int* errorFlag);

/**
 * Analyzes the string directive in the given line.
 *
 * @param line The given line.
 * @param currentLine The current line number (for error message).
 * @param filename The name of the source file (for error message).
 * @param currentLine The current line number.
 * @param filename The name of the source file.
 * @param st The st node of the line.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 * */
static int stringDirAnalyze(const char* line,const char* label,int* index,int currentLine,const char* filename,st_ptr st,int* errorFlag);

/**
 * Analyzes the data directive in the given line.
 *
 * @param line The given line.
 * @param label The label of the directive (if exists).
 * @param index A pointer to the index in the line.
 * @param currentLine The current line number (for error message).
 * @param filename The name of the source file (for error message).
 * @param st The st node of the line.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
static int dataDirAnalyze(const char* line,const char* label,int* index,int currentLine,const char* filename,st_ptr st,int* errorFlag);

/**
 * Analyzes the external labels in the given line.
 *
 * @param definedLabel A label that may be defined before the command
 * @param line The given line.
 * @param symbol_head The head of the symbol table.
 * @param currentLine The current line number.
 * @param filename The name of the source file.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
static int extLabelsAnalyze(const char* definedLabel, const char* line, symbol_ptr* symbol_head, int currentLine, const char* filename, int* errorFlag);

/**
 * Analyzes the entry label in the given line.
 *
 * @param label The label to analyze.
 * @param symbol_head The head of the symbol table.
 * @param currentLine The current line number.
 * @param filename The name of the source file.
 * @param errorFlag A pointer to the error flag.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
static int entryLabelAnalyze(char* label, symbol_ptr* symbol_head, int currentLine, const char* filename, int* errorFlag);

st_ptr lexer(char *filename) {

    FILE *amFile = fopen(filename, "r");
    char line[MAX_LENGTH_LINE];
    char lineCopy[MAX_LENGTH_LINE];
    char *delim = " \t\n";
    int currentLine = 1,lineError=FALSE;
    symbol_ptr symbol_head = NULL;
    st_ptr st_head = NULL;
    if (amFile == NULL)
        return NULL;

    while (fgets(line, MAX_LENGTH_LINE_EXTENDED, amFile)) {

        char* label;
        char definedLabel[MAX_LABEL_SIZE]={NULL_TERM};
        int errorFlag = FALSE, labelFlag=FALSE,dirType,instType;
        int* p_errorFlag = &errorFlag;
        st_ptr st;
        symbol_ptr newSymbol;
        st = (st_ptr) malloc(sizeof(sentenceTree));

        MALLOC_CHECK(st)
        initializeSt(st);

        /*Checks if the line length is greater than the allowed length*/
        if (strlen(line) > MAX_LENGTH_LINE) {
            printf("Error: line %d is too long in file %s\n", currentLine, filename);
            currentLine++;
            errorFlag = TRUE;
        }

        if (errorFlag == FALSE) {
            strcpy(lineCopy, line);
            label = strtok(lineCopy, delim);

            /*In case there's a label*/
            if (label[strlen(label) - 1] == ':'){
                label[strlen(label) - 1] = '\0';

                /*Checks if the symbol is valid, if so, inserts it into the symbol table*/
                if(isValidLabel(label, symbol_head, filename, currentLine,p_errorFlag,relocatable)==TRUE){
                    newSymbol  = createNewSymbol(label,relocatable);
                    addToSymbolTable(&symbol_head,newSymbol);
                    strcpy(st->label,label);

                    strcpy(definedLabel,label);
                    labelFlag=TRUE;
                    st->hasLabel=TRUE;

                    label = strtok(NULL, delim);
                    if(label==NULL){
                        printf("Error: missing command in line %d in %s\n", currentLine,filename);
                        errorFlag=TRUE;
                    }
                }
            }

            /*Checks if that label is a directive*/
            if((dirType = isDirectiveLabel(label))!=non_dir && errorFlag == FALSE){
                st->sentenceType =  directive;
                st->directiveType =  dirType;
                label = strtok(NULL, delim);

                /*If this is a DATA directive, will analyze the line*/
                if(dirType==DATA){
                    int lastValidChar;
                    int tempIndex = findFirstNumber(line,labelFlag,st);
                    lastValidChar = dataDirAnalyze(line,label,&tempIndex,currentLine,filename,st,p_errorFlag);
                    skipWhiteChars(line,&tempIndex);
                    if(!isdigit(lastValidChar) && errorFlag==FALSE){
                        printf("Error: Extraneous text after end of command in line %d in %s\n", currentLine,filename);
                        errorFlag=TRUE;
                    }
                }

                /*If this is a STRING directive, will analyze the line*/
                if(dirType==STRING){
                    int tempIndex = findIndexFromLine(line,APOSTROPHES);
                    stringDirAnalyze(line,label,&tempIndex,currentLine,filename,st,&errorFlag);
                    skipWhiteChars(line,&tempIndex);
                    if(line[tempIndex]!=END_OF_LINE && errorFlag==FALSE){
                        printf("Error: Extraneous text after end of command in line %d in %s\n", currentLine,filename);
                        errorFlag=TRUE;
                    }
                }

                /*If this is a ENTRY/EXTERN directive, will analyze the line*/
                if(dirType == ENTRY || dirType == EXTERN){
                    int type = (dirType==ENTRY)?entry:external;
                    if(labelFlag == TRUE)
                        printf("Warning: The label %s has been defined in line %d  in %s  before .entry/.extern directive\n",definedLabel,currentLine,filename);
                    switch (type) {
                        case entry:
                            entryLabelAnalyze(label,&symbol_head,currentLine,filename,p_errorFlag);
                            break;
                        case external:
                            extLabelsAnalyze(definedLabel,line,&symbol_head,currentLine,filename,p_errorFlag);
                            break;
                    }
                }
            }

            /*In case there's a label*/
            else if((instType = isInstructionLabel(label))!=non_op && errorFlag == FALSE){
                st->sentenceType =  instruction;
                st->opcode =  instType;
                st->numOfOperands = getNumOfOperands(instType);
                label = strtok(NULL, "");
                operandsAnalyze(label,currentLine,filename,st,p_errorFlag);
            }

            /*If no directive or instruction was detected*/
            else {
                if(errorFlag==FALSE){
                    printf("Error: Undefined command name in line %d in %s\n", currentLine,filename);
                    errorFlag=TRUE;
                }
            }
        }
        /*Checks at the end of each line whether there was an error in the line,
             * if so it will indicate that there was an error in some line*/
        if(errorFlag==FALSE)
            addToStTable(&st_head,st);
        else
        {
            if(st!=NULL)free(st);
            lineError=TRUE;
        }
        currentLine++;
        labelFlag=FALSE;
        errorFlag=FALSE;
    }

    fclose(amFile);

    /*If there were no errors in any line*/
    if(lineError==FALSE){
        st_head->symbol_head  = symbol_head;
        return st_head;
    }

    /*if there was an error, frees the tables*/
    else{
        freeStTable(st_head);
        freeSymbolTable(symbol_head);
        return NULL;
    }
}

static int getOperandType(char* operand,st_ptr st,int op_method,int currentLine,const char* filename,int* errorFlag){
    int validOperand = FALSE, type, opNum=0;

    /*If the operand may be a register*/
    if(operand[0]=='@' && strlen(operand)==3) {

        /*Removes the @ character and leaves the rest of the characters*/
        operand[0] = operand[1];
        operand[1] = operand[2];
        operand[2] = '\0';

        /*Checks if the label is a register*/
        if (isRegisterLabel(operand) != non_reg) {
            type = reg;
            opNum = isRegisterLabel(operand);
            validOperand = TRUE;
        }
        else{
            printf("Error: invalid register name in line %d in %s\n",currentLine,filename);
            SET_ERROR
        }
    }

    /*Checks if the operand is a valid number*/
    if(isValidOpNumber(operand) == TRUE && validOperand == FALSE){
        type=number;
        opNum=strtol(operand,NULL,10);
        validOperand=TRUE;
    }

    /*Checks if the operand is a valid label*/
    if(isValidOpLabel(operand)==TRUE && validOperand==FALSE){
        type=label;
        if(op_method==source)
            strcpy(st->sourceOpLabel,operand);
        else
            strcpy(st->destOpLabel,operand);
        validOperand=TRUE;
    }

    /*If not any of them - the operand is not valid*/
    if(validOperand==FALSE){
        printf("Error: invalid operand given in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }
    else{
        switch (op_method) {
            case source:
                st->sourceOpType = type;
                if(type!= label)
                    st->sourceOp = opNum;
                break;
            case destination:
                st->destOpType = type;
                if(type!= label)
                    st->destOp = opNum;
                break;
        }
    }
    return TRUE;
}

static int operandsAnalyze(char* line,int currentLine,const char* filename,st_ptr st,int* errorFlag){
    int numOfOperands = st->numOfOperands, opCode =  st->opcode;
    char op1[MAX_LENGTH_LINE]={'\0'},op2[MAX_LENGTH_LINE]={'\0'};
    char lastValidChar=ZERO_NUMBER,currentChar;
    int  index=0,op1Index=0,op2Index=0,whiteCounts=0,hasComma=FALSE,opCount = 1;
    int  hasOp1=FALSE,hasOp2=FALSE;

    if(line!=NULL){
        skipWhiteChars(line,&index);
        while (1){

            currentChar = line[index];

            if (lastValidChar == COMMA && currentChar == COMMA) {
                printf("Error: multiple commas in line %d in %s\n", currentLine, filename);
                SET_ERROR
            }

            if (lastValidChar != COMMA && currentChar != COMMA &&  whiteCounts>0) {
                printf("Error: missing comma in line %d in %s\n", currentLine, filename);
                SET_ERROR
            }

            /*will enter data according to the appropriate operand (source/destination)*/
            if(currentChar!=COMMA && opCount<3){
                if(hasComma==FALSE){
                    op1[op1Index]  = currentChar;
                    op1Index++;
                    lastValidChar = currentChar;
                    hasOp1=TRUE;
                }
                else{
                    op2[op2Index]  = currentChar;
                    op2Index++;
                    lastValidChar = currentChar;
                    hasOp2=TRUE;
                }
            }
            else{
                hasComma=TRUE;
                lastValidChar=COMMA;
                opCount++;
            }

            index++;
            whiteCounts = skipWhiteChars(line, &index);

            if(line[index]==END_OF_LINE){
                if(lastValidChar!=op1[op1Index-1] && lastValidChar!=op2[op2Index-1] ){
                    printf("Error: Extraneous text after end of command in line %d in %s\n", currentLine,filename);
                    SET_ERROR
                }
                break;
            }
        }
    }
    else{
        if(numOfOperands==0)
            return TRUE;
        printf("Error: operand not given in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }

    /*too many operands error*/
    if((numOfOperands==0 && hasOp1==TRUE) || (numOfOperands==1 && hasOp2==TRUE)){
        printf("Error: too many operands given in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }

    /*too few operands error*/
    if((numOfOperands==1 && hasOp1==FALSE) || (numOfOperands==2 && hasOp2==FALSE)){
        printf("Error: too few operands given in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }

    /*operand/s isn't valid*/
    if(numOfOperands==1){
        if(hasOp1==TRUE && getOperandType(op1,st,destination,currentLine,filename,errorFlag)==FALSE){
            SET_ERROR
        }
    }
    if(numOfOperands==2){
        if((hasOp1==TRUE && getOperandType(op1,st,source,currentLine,filename,errorFlag)==FALSE) ||
           (hasOp2==TRUE && getOperandType(op2,st,destination,currentLine,filename,errorFlag)==FALSE)){
            SET_ERROR
        }
    }
    if(st->destOpType == number  && !(opCode==cmp||opCode==prn||opCode==rts||opCode==stop)){
        printf("Error: a number cannot be a destination operand in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }
    if((st->sourceOpType == number || st->sourceOpType == reg) && opCode==lea){
        printf("Error: a number/register cannot be a source operand in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }
    addressingAnalyze(st);
    return TRUE;
}

static int stringDirAnalyze(const char* line,const char* label,int* index,int currentLine,const char* filename,st_ptr st,int* errorFlag){
    int strIndex = 1;

    /*If a string is not defined or does not start with apostrophes*/
    if(label==NULL || (*index)==0 || label[0]!=APOSTROPHES ){
        printf("Error: a string has been not defined / defined correctly in line %d in %s\n",currentLine,filename);
        SET_ERROR
    }

    (*index)++;

    while (line[(*index)]!=APOSTROPHES){

        if(line[(*index)]==END_OF_LINE){
            printf("Error: missing apostrophes for the string in line %d in %s\n",currentLine,filename);
            SET_ERROR
        }

        else{
            st->directive.String.str[strIndex-1] = line[(*index)];
            strIndex++;
            (*index)++;
        }

    }

    /*If the string is correct, insert the character 0 at the end*/
    if((*errorFlag)==FALSE){
        st->directive.String.str[strIndex-1] = 0;
        (*index)+=1;/*PROMOTE AFTER "*/
    }

    return TRUE;
}

static int dataDirAnalyze(const char* line,const char* label,int* index,int currentLine,const char* filename,st_ptr st,int* errorFlag) {
    int numArrIndex = 0, stNumArr = 0, hasSign = FALSE;
    char numberArr[MAX_LABEL_SIZE]={0};
    char currentChar, lastValidChar = '0';
    int whiteCounts = 0;

    /*If the label does not start with a sign (minus/plus) or number*/
    if (label == NULL || (!isdigit(label[0]) && label[0] != MINUS && label[0] != PLUS) || (*index) == 0) {
        printf("Error: missing/invalid parameter in line %d in %s\n", currentLine, filename);
        SET_ERROR
    }

    while (1) {

        currentChar = line[*index];

        if (currentChar == PLUS || currentChar == MINUS) {
            if (hasSign == TRUE) {
                printf("Error: multiple signs in line %d in %s\n", currentLine, filename);
                SET_ERROR
            }
            hasSign = TRUE;
        }

        if (lastValidChar == COMMA && currentChar == COMMA) {
            printf("Error: multiple commas in line %d in %s\n", currentLine, filename);
            SET_ERROR
        }

        if (lastValidChar != COMMA && currentChar != COMMA &&  whiteCounts>0) {
            printf("Error: missing comma in line %d in %s\n", currentLine, filename);
            SET_ERROR
        }

        /*If the character of the label is valid, will be inserted into the array*/
        if (isdigit(currentChar) || currentChar == MINUS || currentChar == PLUS) {
            if (currentChar != PLUS) {
                hasSign = (currentChar == MINUS) ? TRUE : FALSE;
                numberArr[numArrIndex] = currentChar;
                lastValidChar = currentChar;
                numArrIndex++;
            }
        }

        /*Reaching the end of sub input*/
        if (!isdigit(currentChar) &&  currentChar != MINUS && currentChar!=PLUS) {
            if (currentChar != COMMA) {
                printf("Error: invalid parameter in line %d in %s\n", currentLine, filename);
                SET_ERROR
            } else {
                int number = 0;
                number = strtol(numberArr, NULL, 10);
                if (number <= MAX_VALID_DIR_NUMBER && number >= MIN_VALID_DIR_NUMBER) {
                    st->directive.Data.numArr[stNumArr] = strtol(numberArr, NULL, 10);
                    memset(numberArr, 0, sizeof(numberArr));
                    numArrIndex = 0;
                    stNumArr++;
                    hasSign = FALSE;
                    lastValidChar = currentChar;
                } else {
                    printf("Error: the number %d in line %d in %s is outside the allowed range \n", number, currentLine,filename);
                    SET_ERROR
                }
            }
        }

        (*index)++;
        whiteCounts = skipWhiteChars(line, index);

        /*Reaching the end of a line and handling correct/incorrect input*/
        if(line[*index]==END_OF_LINE){
            if(isdigit(lastValidChar)){
                int number = 0;
                number = strtol(numberArr, NULL, 10);
                if(number==0 && numberArr[0]!=ZERO_NUMBER){
                    printf("Error: invalid parameter in line %d in %s\n", currentLine, filename);
                    SET_ERROR
                }
                else if(number <= MAX_VALID_DIR_NUMBER && number >= MIN_VALID_DIR_NUMBER)
                    st->directive.Data.numArr[stNumArr] = strtol(numberArr, NULL, 10);
                else{
                    printf("Error: the number %d in line %d in %s is outside the allowed range \n", number, currentLine,filename);
                    SET_ERROR
                }
            }
            break;
        }
    }
    return lastValidChar;
}

static int extLabelsAnalyze(const char* definedLabel,const char* line, symbol_ptr* symbol_head, int currentLine, const char* filename, int* errorFlag) {
    int index = 0;
    int labelIndex = 0, whiteCounts = 0;
    char symbolName[MAX_LENGTH_LINE] = { NULL_TERM };
    char currentChar, lastValidChar = NULL_TERM;

    skipWhiteChars(line, &index);
    index+= strlen(definedLabel)+1;
    skipWhiteChars(line, &index);
    index += strlen(".extern");
    skipWhiteChars(line, &index);

    /*If there are no parameters*/
    if (line[index] == END_OF_LINE) {
        printf("Error: missing parameters in line %d in %s\n", currentLine, filename);
        *errorFlag = TRUE;
    }

    while (1) {
        currentChar = line[index];

        if (lastValidChar == COMMA && currentChar == COMMA) {
            printf("Error: multiple commas in line %d in %s\n", currentLine, filename);
            *errorFlag = TRUE;
        }

        if (lastValidChar != COMMA && currentChar != COMMA && whiteCounts > 0) {
            printf("Error: missing comma in line %d in %s\n", currentLine, filename);
            *errorFlag = TRUE;
        }

        /*Reaching a comma, ending input and testing*/
        if (line[index] == COMMA) {
            if (isValidLabel(symbolName, *symbol_head, filename, currentLine, errorFlag, external) == TRUE) {
                symbol_ptr newSymbol = createNewSymbol(symbolName, external);
                addToSymbolTable(symbol_head, newSymbol);
                memset(symbolName, NULL_TERM, sizeof(label));
                labelIndex = 0;
            }
        } else {
            symbolName[labelIndex] = line[index];
            labelIndex++;
        }

        lastValidChar = line[index];
        index++;
        whiteCounts = skipWhiteChars(line, &index);

        /*Handling valid/invalid input at the end of a line*/
        if (line[index] == END_OF_LINE) {
            if (symbolName[0] != NULL_TERM) {
                if (isValidLabel(symbolName, *symbol_head, filename, currentLine, errorFlag, external) == TRUE) {
                    symbol_ptr newSymbol = createNewSymbol(symbolName, external);
                    addToSymbolTable(symbol_head, newSymbol);
                }
            } else if (currentChar == COMMA) {
                printf("Error: missing parameters in line %d in %s\n", currentLine, filename);
                *errorFlag = TRUE;
            }

            break;
        }
    }
    return TRUE;
}

static int entryLabelAnalyze(char* label, symbol_ptr* symbol_head, int currentLine, const char* filename, int* errorFlag) {
    symbol_ptr tempSymbol, newSymbol;
    char* delim = " \t\n";

    /*Only one label is allowed, so straight away will check it*/
    if (isValidLabel(label, *symbol_head, filename, currentLine, errorFlag, entry) == TRUE) {
        tempSymbol = searchForSymbol(symbol_head, label);
        if (tempSymbol != NULL)
            tempSymbol->type = entry;
        else {
            newSymbol = createNewSymbol(label, entry);
            addToSymbolTable(symbol_head, newSymbol);
        }
    }

    label = strtok(NULL, delim);

    /*Checks for extra text at the end of a line*/
    if (label != NULL && (*errorFlag) == FALSE) {
        printf("Error: Extraneous text after end of command in line %d in %s\n", currentLine, filename);
        *errorFlag = TRUE;
    }

    return TRUE;
}
