#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "secondPass.h"
#include "globals.h"
#include "utils.h"

/**
 * Builds the word for an instruction label.
 * Updates the label address and ARE values in the instruction word.
 *
 * @param labelAddress The array containing the label address.
 * @param areArr The array containing the ARE values.
 * @param insWord The instruction word to update.
 */
static void buildWordForInsLabel(int labelAddress[], int areArr[], wordIns_ptr insWord);

/**
 * Resets the arrays used in the second pass to their initial values.
 *
 * @param labelAddress The array containing the label addresses.
 * @param areArr The array containing the ARE values.
 */
static void resetSecondPassArrays(int labelAddress[], int areArr[]);

/**
 * Add the address for a label operand.
 *
 * @param symbol_head The head of the symbol table.
 * @param wordIns_head The head of the instruction words.
 */
static void addressForLabels(symbol_ptr symbol_head, wordIns_ptr wordIns_head);

/**
 * Creates the object (output) file.
 *
 * @param file The file name.
 * @param wordTable_head The head of the word table.
 * @return A pointer to the created object file name.
 */
static char* createObFile(char* file, wordTable_ptr wordTable_head);

/**
 * Creates the entry file.
 *
 * @param file The file name.
 * @param symbol_head The head of the symbol table.
 * @return 1 if the entry file was created successfully, 0 otherwise.
 */
static int createEntryFile(char* file, symbol_ptr symbol_head);

/**
 * Creates the extern file.
 *
 * @param file The file name.
 * @param symbol_head The head of the symbol table.
 * @param wordIns_head The head of the instruction words.
 * @return A pointer to the created extern file name.
 */
static char* createExternFile(char* file, symbol_ptr symbol_head, wordIns_ptr wordIns_head);

void secondPass(symbol_ptr symbol_head,wordTable_ptr wordTable_head,char* file){
    char* entFile = NULL;
    char* obFile = NULL;
    char* extFile = NULL;
    int entReturn;

    /*in case  of an error in lexer*/
    if(wordTable_head==NULL)return;

    /*Finishes defining label words*/
    addressForLabels(symbol_head,wordTable_head->ins_head);

    /*Creates an object file*/
    obFile = createObFile(file,wordTable_head);

    /*Creates an entry file*/
    entReturn = createEntryFile(file,symbol_head);

    /*Creates an extern file (if not defined, not created)*/
    extFile = createExternFile(file,symbol_head,wordTable_head->ins_head);

    entFile = setOutputFile(file,".ent");

    switch (entReturn) {

        /*If no entry has been defined, the file will be deleted*/
        case 0:
            remove(entFile);
            SAFE_FREE(entFile)
            SAFE_FREE(obFile)
            SAFE_FREE(extFile)
            break;

        /*If there is an error in the entry file (a label was defined as
         * an entry but was not defined later in the file)
         * All the files created in the second pass will be deleted */
        case -1:
            remove(entFile);
            SAFE_FREE(entFile)

            if(obFile!=NULL){remove(obFile);}
            SAFE_FREE(obFile)

            if(extFile!=NULL){remove(extFile);}
            SAFE_FREE(extFile)

            break;

        /*All files are fine, will release the memory allocated to them*/
        default:
            SAFE_FREE(entFile)
            SAFE_FREE(obFile)
            SAFE_FREE(extFile)
            break;
    }
}

static void addressForLabels(symbol_ptr symbol_head,wordIns_ptr wordIns_head){
    int labelAddress[12]={0},areArr[2]={0};
    symbol_ptr tempSymbol = NULL;
    wordIns_ptr tempWordIns = wordIns_head;

    /*for ins list, data and string cannot get labels as operands*/
    while (tempWordIns!=NULL){

        /*If the word is of a label*/
        if(tempWordIns->isLabel==TRUE){
            tempSymbol = searchForSymbol(&symbol_head,tempWordIns->labelName);

            /*If the word was defined as an entry and was not defined in the file*/
            if(tempSymbol==NULL){
                printf("Error: the symbol %s is not defined as external label or in the source file\n",tempWordIns->labelName);
                return;
            }

            /*Creates the word for external label*/
            if(tempSymbol->type==external){
                areArr[0]=1;
                buildWordForInsLabel(labelAddress,areArr,tempWordIns);
            }

            /*Creates the word for entry label*/
            else{
                areArr[1]=1;
                decimalToBinary(labelAddress,tempSymbol->address,10);
                buildWordForInsLabel(labelAddress,areArr,tempWordIns);
            }

            decimalToBinary(labelAddress,tempSymbol->address,10);
        }
        resetSecondPassArrays(labelAddress,areArr);
        tempWordIns=tempWordIns->next;
    }
}

static void resetSecondPassArrays(int labelAddress[],int areArr[]){
    /*Initializes the arrays*/
    memset(areArr, 0, 2*sizeof(int));
    memset(labelAddress, 0, 12*sizeof(int));
}

static void buildWordForInsLabel(int labelAddress[],int areArr[],wordIns_ptr insWord){
    /*Builds the word for the label according to the address and ARE*/
    memcpy(insWord->binCode, areArr, 2 * sizeof(int));
    memcpy(insWord->binCode+2, labelAddress, 10 * sizeof(int));
}

static char* createObFile(char* file,wordTable_ptr wordTable_head){
    wordIns_ptr tempInsWord = wordTable_head->ins_head;
    wordDir_ptr tempDirWord = wordTable_head->dir_head;

    /*Creates a new file with the name of the input file and with the extension .ob*/
    char* nameFileOb = setOutputFile(file,".ob");
    FILE * obFile = fopen(nameFileOb,"w");
    if (obFile == NULL) {return NULL;}
    if(tempInsWord==NULL && tempDirWord==NULL){remove(nameFileOb);}

    /*Prints the instruction counter and directive counter to the file*/
    fprintf(obFile,"%d %d\n",wordTable_head->IC,wordTable_head->DC);

    /*Encoder prints to a file every word in the instruction table*/
    while (tempInsWord!=NULL){
        convertToCharsBase64(tempInsWord->binCode,obFile);
        tempInsWord=tempInsWord->next;
    }

    /*Encoder prints to a file every word in the directive table*/
    while (tempDirWord!=NULL){
        convertToCharsBase64(tempDirWord->binCode,obFile);
        tempDirWord=tempDirWord->next;
    }

    fclose(obFile);
    return nameFileOb;
}

static int createEntryFile(char* file, symbol_ptr symbol_head){
    symbol_ptr temp1Symbol = symbol_head;
    char* nameFileEntry = setOutputFile(file,".ent");
    char* amFilename = setOutputFile(file,".am");
    int entrySymbolFound = FALSE,count=0;

    /*Creates a new file with the name of the input file and with the extension .ent*/
    FILE * entFile = fopen(nameFileEntry,"w");
    if (entFile == NULL) {
        SAFE_FREE(nameFileEntry)
        SAFE_FREE(amFilename)
        return 1;
    }
    if(symbol_head==NULL){remove(nameFileEntry);}

    /*Goes through all the symbols in the symbol table and checks if they have been defined as entry*/
    while (temp1Symbol!=NULL){
        if(temp1Symbol->type==entry){
            symbol_ptr temp2symbol = symbol_head;

            /*Prints the name and address of that symbol to a file*/
            while (temp2symbol!=NULL) {
                if (strcmp(temp2symbol->name, temp1Symbol->name) == 0 && temp2symbol->address != 0) {
                    fprintf(entFile, "%s\t%d\n", temp2symbol->name,temp2symbol->address);
                    entrySymbolFound = TRUE;
                    count++;
                }
                temp2symbol = temp2symbol->next;

                /*If the word was defined as an entry and was not defined in the file*/
                if (temp2symbol == NULL && entrySymbolFound == FALSE) {
                    printf("Error: the label %s defined as entry, but didn't defined in file %s\n", temp1Symbol->name,amFilename);
                    SAFE_FREE(nameFileEntry)
                    SAFE_FREE(amFilename)
                    fclose(entFile);
                    return -1;
                }
            }
        }
        temp1Symbol=temp1Symbol->next;
    }

    /*If no entry symbols were defined in the file*/
    if(count==0){
        remove(nameFileEntry);
        SAFE_FREE(nameFileEntry)
        SAFE_FREE(amFilename)
        fclose(entFile);
        return 0;
    }

    fclose(entFile);
    SAFE_FREE(nameFileEntry)
    SAFE_FREE(amFilename)
    return 1;

    /*return type:
     * 1: everything is good
     * 0: count = 0 - ent file will be removed
     * -1: for entry error - all file will be removed
     * */
}

static char* createExternFile(char* file, symbol_ptr symbol_head,wordIns_ptr wordIns_head){
    symbol_ptr temp1Symbol = symbol_head;
    char* nameFileExtern = setOutputFile(file,".ext");
    int count=0,currentAddress = ADDRESS_START;

    /*Creates a new file with the name of the input file and with the extension .ext*/
    FILE * extFile = fopen(nameFileExtern,"w");
    if (extFile == NULL) {
        SAFE_FREE(nameFileExtern)
        return NULL;}

    if(symbol_head==NULL){remove(nameFileExtern);}

    /*Goes through all the symbols in the symbol table and checks if they have been defined as extern*/
    while (temp1Symbol!=NULL){
        if(temp1Symbol->type==external){
            wordIns_ptr tempWord = wordIns_head;

            /*Goes through all the words and checks if the word is of an
             * extern label, and it is enough to file the name and address*/
            while (tempWord!=NULL) {
                if (strcmp(tempWord->labelName, temp1Symbol->name) == 0) {
                    fprintf(extFile, "%s\t%d\n", temp1Symbol->name,currentAddress);
                    count++;
                }
                tempWord = tempWord->next;
                currentAddress++;
            }
        }
        currentAddress=ADDRESS_START;
        temp1Symbol=temp1Symbol->next;
    }

    /*If no extern symbols were defined in the file*/
    if(count==0){
        remove(nameFileExtern);
        fclose(extFile);
        SAFE_FREE(nameFileExtern)
        return NULL;
    }
    fclose(extFile);
    return nameFileExtern;
}










