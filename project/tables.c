#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tables.h"
#include "globals.h"


void initializeInsWord(wordIns_ptr word){

    /*Initializes variables to certain values*/
    word->isLabel=FALSE;
    word->address=0;
    word->hasLabel=FALSE;
    memset(word->labelName,NULL_TERM,MAX_LABEL_SIZE);
    memset(word->binCode,0,WORD_NUM_OF_BITS);
    word->next=NULL;
}

void initializeDirWord(wordDir_ptr word){

    /*Initializes variables to certain values*/
    word->address=0;
    word->hasLabel=FALSE;
    memset(word->labelName,NULL_TERM,MAX_LABEL_SIZE);
    memset(word->binCode,0,WORD_NUM_OF_BITS);
    word->next=NULL;
}

void initializeSt(st_ptr st){

    /*Initializes variables to certain values*/
    int i;
    st->hasLabel=FALSE;
    st->sentenceType=FALSE;
    st->directiveType=FALSE;
    st->opcode=FALSE;
    st->numOfOperands=0;
    st->sourceOpType=0;
    st->sourceOp=0;
    st->destOpType=0;
    st->destOp=0;
    st->sourceAdrMethod=0;
    st->destAdrMethod=0;
    for (i = 0; i < MAX_LENGTH_LINE; i++)
        st->directive.Data.numArr[i] = NUM_OUT_OF_RANGE;
}

symbol_ptr searchForSymbol(symbol_ptr* head,char* symbolName){

    /*Searches for a symbol in a symbol table and returns its address (if found)*/
    symbol_ptr temp = *head;
    while (temp!=NULL){
        if(strcmp(temp->name,symbolName)==TRUE)
            return temp;
        temp=temp->next;
    }
    return NULL;
}

void addToSymbolTable(symbol_ptr* head, symbol_ptr newSymbol) {

    /*Adds a symbol to the symbol table*/
    symbol_ptr temp = *head;
    newSymbol->next=NULL;
    if (temp == NULL) {
        *head = newSymbol;
        (*head)->next = NULL;
    } else {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newSymbol;
    }
}

symbol_ptr createNewSymbol(char* name,int type){

    /*Creates a new symbol, initializes its variables and returns its address*/
    symbol_ptr newSymbol  = (symbol_ptr)malloc(sizeof(symbolTable));
    if(newSymbol==NULL){ printf("cannot allocate memory");return NULL;}
    strcpy(newSymbol->name,name);
    newSymbol->address=0;
    newSymbol->type=type;
    newSymbol->next=NULL;
    return newSymbol;
}

void addToStTable(st_ptr* head, st_ptr st) {
    st_ptr temp = *head;
    st->next=NULL;

    /*Adds a st to the st table*/
    if (temp == NULL) {
        *head = st;
        (*head)->next = NULL;
    } else {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = st;
    }
}

void addToInsWordTable(wordIns_ptr * head, wordIns_ptr word){
    wordIns_ptr temp = *head;
    word->next=NULL;

    /*Inserts an instruction word into an instruction word table*/
    if (temp == NULL) {
        *head = word;
        (*head)->next = NULL;
    } else {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = word;
    }
}



void addToDirWordTable(wordDir_ptr * head, wordDir_ptr word){
    wordDir_ptr temp = *head;

    word->next=NULL;

    /*Inserts an directive word into an directive word table*/
    if (temp == NULL) {
        *head = word;
        (*head)->next = NULL;
    } else {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = word;
    }
}



void freeStTable(st_ptr head){
    st_ptr temp;

    /*Frees all st table nodes*/
    while (head!=NULL){
        temp=head;
        head=head->next;
        free(temp);
    }
}

void freeSymbolTable(symbol_ptr head){
    symbol_ptr temp;

    /*Frees all symbol table nodes*/
    while (head!=NULL){
        temp=head;
        head=head->next;
        free(temp);
    }
}

void freeInsTable(wordIns_ptr head){
    wordIns_ptr temp;

    /*Frees all Instruction table nodes*/
    while (head!=NULL){
        temp=head;
        head=head->next;
        free(temp);
    }
}

void freeDirTable(wordDir_ptr head){
    wordDir_ptr temp;

    /*Frees all Directive table nodes*/
    while (head!=NULL){
        temp=head;
        head=head->next;
        free(temp);
    }
}

void freeWordsTable(wordTable_ptr head){
    wordIns_ptr insHead;
    wordDir_ptr dirHead;
    wordIns_ptr tempInsWord;
    wordDir_ptr tempDirWord;

    /*In case of  an error in the first pass, will be NULL*/
    if(head==NULL)return;

    insHead = head->ins_head;
    dirHead = head->dir_head;

    /*Frees the table of instruction and directive words*/
    while (insHead!=NULL){
        tempInsWord=insHead;
        insHead=insHead->next;
        free(tempInsWord);
    }
    while (dirHead!=NULL){
        tempDirWord=dirHead;
        dirHead=dirHead->next;
        free(tempDirWord);
    }

    /*Releases the node allocated to hold the pointers to the tables*/
    free(head);
}

void printMacroToFile(FILE* program,FILE* newFile, int macroIndex,macroPtr head){
    int i,currentLine=1;
    char* line;
    macroPtr pMcr = head;

    line = (char *) malloc(MAX_LENGTH_LINE_EXTENDED);
    fseek(program, 0, SEEK_SET);

    if (line == NULL) {
        printf("\ncannot allocated memory\n");
        exit(1);
    }

    /*Advances the pointer to point to the link of the right macro*/
    for (i = 0; i < macroIndex; i++)
        pMcr=pMcr->next;

    /*Prints to the am file the lines that the macro contains, excluding blank lines or comment lines*/
    while (fgets(line, MAX_LENGTH_LINE_EXTENDED, program)){
        if(currentLine>=pMcr->startLine && currentLine<=pMcr->endingLine)
        {

            /*Checks whether the line is an empty line or a comment line*/
            char firstChar = '\0';
            for (i = 0; line[i] != '\0'; i++) {
                if (!isspace(line[i])) {
                    firstChar = line[i];
                    break;
                }
            }

            /*Prints the line to the file*/
            if(firstChar!=END_OF_LINE && firstChar!=COMMENT)
                fprintf(newFile, "%s", line);
        }
        currentLine++;
    }

    /*Releases the memory allocated to the line*/
    free(line);
}

int isMacro(macroPtr head,const char name[]){
    int index=0;
    macroPtr temp=head;

    /*Goes through the macro table and checks if a macro exists*/
    while (temp!=NULL){
        if(strcmp(temp->name,name)==TRUE)
            return index;
        temp=temp->next;
        index++;
    }
    return FALSE;
}

void freeMacroTable(macroPtr head){
    macroPtr temp;

    /*Frees all macro table nodes*/
    while (head!=NULL){
        temp=head;
        head=head->next;
        free(temp->name);
        free(temp);
    }
}