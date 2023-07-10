#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "firstPass.h"
#include "globals.h"
#include "utils.h"

/**
 * Builds the first word for an instruction in the word table.
 *
 * @param sourceAdrArr The array representing the source addressing method.
 * @param destAdrArr The array representing the destination addressing method.
 * @param opcodeArr The array representing the opcode.
 * @param areArr The array representing the ARE (Absolute/Relative/External) attribute.
 * @param word A pointer to the word instruction structure.
 */
static void buildFirstWordForIns(int sourceAdrArr[], int destAdrArr[], int opcodeArr[], int areArr[], wordIns_ptr word);

/**
 * Builds a word for a numeric instruction in the word table.
 *
 * @param numInsArr The array representing the numeric instruction.
 * @param areArr The array representing the ARE (Absolute/Relative/External) attribute.
 * @param word A pointer to the word instruction structure.
 */
static void buildWordForNum(int numInsArr[], int areArr[], wordIns_ptr word);

/**
 * Builds a word for a register instruction in the word table.
 *
 * @param sourceReg The array representing the source register.
 * @param destReg The array representing the destination register.
 * @param areArr The array representing the ARE (Absolute/Relative/External) attribute.
 * @param word A pointer to the word instruction structure.
 */
static void buildWordForReg(int sourceReg[], int destReg[], int areArr[], wordIns_ptr word);

/**
 * Updates the addresses of directive words.
 *
 * @param wordDir_head The head pointer of the directives word table.
 * @param IC The final instruction counter.
 */
static void addressingDirWords(wordDir_ptr wordDir_head,int IC);

/**
 * Updates the symbol addresses defined in the instructions.
 *
 * @param wordIns_head The head pointer of the instructions word table.
 * @param symbol_head The head pointer of the symbol table.
 */
static void addressingInsSymbols(wordIns_ptr wordIns_head,symbol_ptr symbol_head);

/**
 * Updates the symbol addresses defined in the directives.
 *
 * @param wordDir_head The head pointer of the directives word table.
 * @param symbol_head The head pointer of the symbol table.
 */
static void addressingDirSymbols(wordDir_ptr wordDir_head,symbol_ptr symbol_head);

wordTable_ptr firstPass(st_ptr st_head,symbol_ptr symbol_head,char* outputName){

    int  currentAddress = ADDRESS_START;    /*start at 100 always*/
    int DC=0,IC=0;  /*instruction counter and data counter*/
    st_ptr tempSt = st_head;
    int srcAndDesRegisters=FALSE;   /*If the 2 operands are registers*/
    int errorFlag  = FALSE;
    wordTable_ptr wordTable_head  =  NULL;
    wordDir_ptr wordDir_head  =  NULL;
    wordIns_ptr wordIns_head  =  NULL;

    /*if there was an error in the lexer, all freed, then it NULL*/
    if(st_head==NULL)return NULL;

    wordTable_head = (wordTable_ptr) malloc(sizeof(wordTable));
    MALLOC_CHECK(wordTable_head)

    /*tempSt - every st that analyzed a line*/
    while (tempSt!=NULL){

        /*If the line was an instruction*/
        if(tempSt->sentenceType==instruction){

            /*Auxiliary arrays to be used for conversion to binary*/
            int sourceAdrArr[3]={0},destAdrArr[3]={0},opcodeArr[4]={0},areArr[2]={0};
            int numInsArr[10]={0},srcReg[5]={0},desReg[5]={0};
            wordIns_ptr word0;

            /*Creates the first word (will always be created in the case of
             * an instruction regardless of the number of operands) */
            int numOfOperands  = tempSt->numOfOperands;
            word0 = (wordIns_ptr) malloc(sizeof(wordIns));
            MALLOC_CHECK(word0)
            initializeInsWord(word0);

            if(tempSt->hasLabel==TRUE){
                word0->hasLabel=TRUE;
                strcpy(word0->labelName,tempSt->label);
            } else word0->hasLabel=FALSE;

            /*Converts the addressing method of the operands according to the number of operands*/
            switch (numOfOperands) {
                case 1:
                    decimalToBinary(destAdrArr,tempSt->destAdrMethod,3);
                    break;
                case 2:
                    decimalToBinary(sourceAdrArr,tempSt->sourceAdrMethod,3);
                    decimalToBinary(destAdrArr,tempSt->destAdrMethod,3);
                    break;

            }

            /*Builds the first word and puts it in the instruction word table*/
            decimalToBinary(opcodeArr,tempSt->opcode,4);
            buildFirstWordForIns(sourceAdrArr,destAdrArr,opcodeArr,areArr,word0);
            word0->address=currentAddress;
            addToInsWordTable(&wordIns_head,word0);

            currentAddress++;
            IC++;
            MEM_CHECK
            resetInsArr(sourceAdrArr,destAdrArr,opcodeArr,areArr,numInsArr,srcReg,desReg);

            /*If the number of operands is 2, will build a word for the source operand
             * (because the destination operand will be built anyway later)*/
            if(numOfOperands==2){
                wordIns_ptr op2word;
                op2word = (wordIns_ptr) malloc(sizeof(wordIns));
                MALLOC_CHECK(op2word)
                initializeInsWord(op2word);
                op2word->isLabel = (tempSt->sourceOpType == label)?TRUE:FALSE;

                /*Builds the word according to the type of operand*/
                switch (tempSt->sourceOpType) {
                    case number:
                        decimalToBinary(numInsArr,tempSt->sourceOp,10);
                        buildWordForNum(numInsArr,areArr,op2word);
                        break;
                    case reg:
                        if(tempSt->destOpType==reg){
                            srcAndDesRegisters=TRUE;
                            decimalToBinary(desReg,tempSt->destOp,5);
                            decimalToBinary(srcReg,tempSt->sourceOp,5);
                        }
                        else
                            decimalToBinary(srcReg,tempSt->sourceOp,5);
                        buildWordForReg(srcReg,desReg,areArr,op2word);
                        break;
                    case label:
                        memset(op2word->binCode,0,sizeof(op2word->binCode));
                        strcpy(op2word->labelName,tempSt->sourceOpLabel);
                        break;
                }

                /*Finishes building the word and puts it in the word table of instructions*/
                op2word->address=currentAddress;
                addToInsWordTable(&wordIns_head,op2word);

                currentAddress++;
                IC++;
                MEM_CHECK
                resetInsArr(sourceAdrArr,destAdrArr,opcodeArr,areArr,numInsArr,srcReg,desReg);
            }

            /*Build the word for the destination operand*/
            if((numOfOperands==1 || numOfOperands==2) && srcAndDesRegisters==FALSE){
                wordIns_ptr op1word;
                op1word = (wordIns_ptr) malloc(sizeof(wordIns));
                MALLOC_CHECK(op1word)
                initializeInsWord(op1word);
                op1word->isLabel = (tempSt->destOpType == label)?TRUE:FALSE;

                /*Builds the word according to the type of operand*/
                switch (tempSt->destOpType) {
                    case number:
                        decimalToBinary(numInsArr,tempSt->destOp,10);
                        buildWordForNum(numInsArr,areArr,op1word);
                        break;
                    case reg:
                        decimalToBinary(desReg,tempSt->destOp,5);
                        buildWordForReg(srcReg,desReg,areArr,op1word);
                        break;
                    case label:
                        memset(op1word->binCode,0,sizeof(op1word->binCode));
                        strcpy(op1word->labelName,tempSt->destOpLabel);
                        break;
                }

                /*Finishes building the word and puts it in the word table of instructions*/
                op1word->address=currentAddress;
                addToInsWordTable(&wordIns_head,op1word);

                currentAddress++;
                IC++;
                MEM_CHECK
                resetInsArr(sourceAdrArr,destAdrArr,opcodeArr,areArr,numInsArr,srcReg,desReg);
            }
        }

        /*If the line is a directive line and it is a DATA or STRING directive*/
        if(tempSt->sentenceType==directive && (tempSt->directiveType==DATA || tempSt->directiveType==STRING)){

            /*it it's DATA directive*/
            if(tempSt->directiveType==DATA){

                /*An auxiliary array for the binary representation of the numbers*/
                int numArr[12]={0}, index=0;

                while (tempSt->directive.Data.numArr[index]!=NUM_OUT_OF_RANGE)
                {
                    /*Creates a new word each time for a new number and puts it in the directive word table*/
                    wordDir_ptr newWord = (wordDir_ptr) malloc(sizeof(wordDir));
                    MALLOC_CHECK(newWord)
                    initializeDirWord(newWord);
                    if(tempSt->hasLabel==TRUE && index==0){
                        newWord->hasLabel=TRUE;
                        strcpy(newWord->labelName,tempSt->label);
                    } else newWord->hasLabel=FALSE;
                    decimalToBinary(numArr,tempSt->directive.Data.numArr[index],12);
                    memcpy(newWord->binCode, numArr, 12 * sizeof(int));
                    newWord->address=DC;
                    addToDirWordTable(&wordDir_head,newWord);

                    DC++;
                    index++;
                    MEM_CHECK
                }
            }

            /*it it's STRING directive*/
            if(tempSt->directiveType==STRING){

                /*An auxiliary array for the binary representation of the chars*/
                int charArr[12]={0},index=0;

                /*Creates a word for the character 0 that comes at the end of each STRING*/
                wordDir_ptr wordFor0str = (wordDir_ptr) malloc(sizeof(wordDir));
                MALLOC_CHECK(wordFor0str)
                initializeDirWord(wordFor0str);
                wordFor0str->hasLabel = FALSE;

                while (tempSt->directive.String.str[index]!=0)
                {
                    /*Creates a new word for each character in the string and converts its ascii code to binary
                     *and puts it in the directive word table */
                    wordDir_ptr newWord = (wordDir_ptr) malloc(sizeof(wordDir));
                    MALLOC_CHECK(newWord)
                    initializeDirWord(wordFor0str);
                    if(tempSt->hasLabel==TRUE && index==0){
                        newWord->hasLabel=TRUE;
                        strcpy(newWord->labelName,tempSt->label);
                    } else newWord->hasLabel=FALSE;
                    charToBinary(tempSt->directive.String.str[index],charArr);
                    memcpy(newWord->binCode, charArr, 12 * sizeof(int));
                    newWord->address=DC;
                    addToDirWordTable(&wordDir_head,newWord);

                    DC++;
                    index++;
                    MEM_CHECK
                }

                /*Sets the word to the character 0, and puts it in the directive word table*/
                memset(charArr,0,12* sizeof(int));
                memcpy(wordFor0str->binCode, charArr, 12 * sizeof(int));
                wordFor0str->address=DC;
                addToDirWordTable(&wordDir_head,wordFor0str);

                DC++;
                MEM_CHECK
            }
        }
        tempSt=tempSt->next;
        srcAndDesRegisters=FALSE;
    }

    addressingDirWords(wordDir_head,IC);
    addressingInsSymbols(wordIns_head,symbol_head);
    addressingDirSymbols(wordDir_head,symbol_head);

    if(errorFlag==TRUE){
        freeInsTable(wordIns_head);
        freeDirTable(wordDir_head);
        return NULL;
    }

    /*Inserts a pointer to the top of the table of instructions
     *and directive, and also their counter*/
    wordTable_head->dir_head = wordDir_head;
    wordTable_head->ins_head = wordIns_head;
    wordTable_head->IC=IC;
    wordTable_head->DC=DC;
    return wordTable_head;
}


static void addressingInsSymbols(wordIns_ptr wordIns_head,symbol_ptr symbol_head){
    wordIns_ptr tempIns=wordIns_head;
    while (tempIns!=NULL){
        if(tempIns->hasLabel==TRUE &&tempIns->isLabel==FALSE){
            symbol_ptr tempSymbol = searchForSymbol(&symbol_head,tempIns->labelName);
            /*it may be null when symbol is external*/
            if(tempSymbol!=NULL)
                tempSymbol->address = tempIns->address;
        }
        tempIns=tempIns->next;
    }
}

static void addressingDirSymbols(wordDir_ptr wordDir_head,symbol_ptr symbol_head){
    wordDir_ptr tempDir = wordDir_head;
    while (tempDir!=NULL){
        if(tempDir->hasLabel==TRUE){
            symbol_ptr tempSymbol = searchForSymbol(&symbol_head,tempDir->labelName);
            /*it may be null when symbol is external*/
            if(tempSymbol!=NULL)
                tempSymbol->address = tempDir->address;
        }
        tempDir=tempDir->next;
    }
}

static void addressingDirWords(wordDir_ptr wordDir_head,int IC){
    wordDir_ptr tempDir = wordDir_head;
    while (tempDir!=NULL){
        tempDir->address=IC+tempDir->address+ADDRESS_START;
        tempDir=tempDir->next;
    }
}

static void buildWordForNum(int numInsArr[],int areArr[],wordIns_ptr word){
    /*builds the binary of the number: 2 bits - are, 10 bits binary representation of a number*/
    memcpy(word->binCode, areArr, 2 * sizeof(int));
    memcpy(word->binCode+2, numInsArr, 10 * sizeof(int));
}
static void buildWordForReg(int sourceReg[],int destReg[],int areArr[], wordIns_ptr word){
    /*builds the binary of a register: bits 0-1 - are, bits 2-6 dest reg, bits 7-11 source reg*/
    memcpy(word->binCode, areArr, 2 * sizeof(int));
    memcpy(word->binCode+2, destReg, 5 * sizeof(int));
    memcpy(word->binCode+7, sourceReg, 5 * sizeof(int));
}

static void buildFirstWordForIns(int sourceAdrArr[],int destAdrArr[],int opcodeArr[],int areArr[],wordIns_ptr word){
    /*builds the binary of the first  instruction word: bits 0-1 - are, bits 2-4 and 9-11 for
     *addressing method, bits 5-9 for opcode in binary*/
    memcpy(word->binCode, areArr, 2 * sizeof(int));
    memcpy(word->binCode + 2, destAdrArr, 3 * sizeof(int));
    memcpy(word->binCode + 5, opcodeArr, 4 * sizeof(int));
    memcpy(word->binCode + 9, sourceAdrArr, 3 * sizeof(int));
}




