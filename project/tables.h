#ifndef TABLES_H
#define TABLES_H

#include <stdio.h>

#define MAX_LENGTH_LINE_EXTENDED 200 /*Maximum line length before valid line length check*/
#define MAX_LENGTH_LINE 81 /*include '\n' and '\000' at the end*/
#define MAX_LABEL_SIZE 31 /*include '\000' at the end*/
#define WORD_NUM_OF_BITS 12 /*Number of bits of a word*/
#define NUM_OUT_OF_RANGE 5000 /*An out-of-range number used to check for a stop condition*/

/*Word table for instruction words*/
typedef struct wordIns * wordIns_ptr;
typedef struct wordIns{

    /*The binary representation of the word*/
    int binCode[WORD_NUM_OF_BITS];

    /*Is the word of a label*/
    int hasLabel;

    /*Is the word of a label*/
    int isLabel;

    /*The label for which the word was created*/
    char labelName[MAX_LABEL_SIZE];

    /*The word address*/
    int address;

    /*Pointer to the next word*/
    wordIns_ptr next;

} wordIns;

/*Word table for directive words*/
typedef struct wordDir * wordDir_ptr;
typedef struct wordDir{

    /*The binary representation of the word*/
    int binCode[WORD_NUM_OF_BITS];

    /*Is the word of a label*/
    int hasLabel;

    /*The label for which the word was created*/
    char labelName[MAX_LABEL_SIZE];

    /*The word address*/
    int address;

    /*Pointer to the next word*/
    wordDir_ptr next;

} wordDir;

/*A structure that holds the instructions and directive counter,
 *and points to the head of the instructions table and the directive table*/
typedef struct wordTable * wordTable_ptr;
typedef struct wordTable{

    /*Pointer to the head of an instruction table*/
    wordIns_ptr ins_head;

    /*Pointer to the head of an directive table*/
    wordDir_ptr dir_head;

    /*instruction counter*/
    int IC;

    /*directive counter*/
    int DC;

} wordTable;

/*Macro table for macros*/
typedef struct macro * macroPtr;
typedef struct macro{

    /*The name of the macro*/
    char* name;

    /*The line that the macro starts*/
    int startLine;

    /*The line that the macro ends*/
    int endingLine;

    /*Pointer to the next macro*/
    macroPtr next;

}macro;

/*Symbol table for symbols*/
typedef struct symbolTable * symbol_ptr;
typedef struct symbolTable{

    /*The name of the symbol*/
    char name[MAX_LABEL_SIZE];

    /*The address of the symbol*/
    int address;

    /*The symbol type (defined in file/entry/external)*/
    int type;

    /*Pointer to the next symbol*/
    symbol_ptr next;

}symbolTable;

/*sentenceTree table for sentenceTree node (each node represent a line)*/
typedef struct sentenceTree * st_ptr;
typedef struct sentenceTree{

    /*The name of the label defined in the row (if any)*/
    char label[MAX_LABEL_SIZE];

    /*Has a label been added to the line*/
    unsigned short hasLabel;

    /*Line type (instruction line/ Directive line)*/
    unsigned short sentenceType;

    /*for directive sentence*/

    /*Directive type (DATA/STRING/ENTRY/EXTERN)*/
    unsigned short directiveType;
    union {
        struct dataDirective {

            /*An array that holds legal numbers defined in the directive*/
            int numArr[MAX_LENGTH_LINE];

        } Data;
        struct stringDirective {

            /*An array that holds a legal string defined in the directive*/
            char str[MAX_LENGTH_LINE];

        } String;
    } directive;

    /*for instruction sentence*/

    /*A number that represents an opcode type*/
    unsigned short opcode;

    /*The number of operands according to the opcode*/
    unsigned short numOfOperands;

    /*operand type if present (number/label/register)*/
    unsigned short sourceOpType;

    /*In case the operand is a number or a register, represent the number or the register number*/
    int sourceOp;

    /*operand type if present (number/label/register)*/
    unsigned short destOpType;

    /*In case the operand is a number or a register, represent the number or the register number*/
    int destOp;

    /*In case and the source operand is a label*/
    char sourceOpLabel[MAX_LABEL_SIZE];

    /*In case and the destination operand is a label*/
    char destOpLabel[MAX_LABEL_SIZE];

    /*The addressing method for the source operand*/
    unsigned short sourceAdrMethod;

    /*The addressing method for the destination operand*/
    unsigned short destAdrMethod;

    /*points to the next node*/
    st_ptr next;

    /*Pointer to the head of the symbol table*/
    symbol_ptr symbol_head;

}sentenceTree;


/**
 * Adds a symbol to the symbol table.
 *
 * @param head A pointer to the head of the symbol table.
 * @param newSymbol The symbol to be added.
 */
void addToSymbolTable(symbol_ptr* head, symbol_ptr newSymbol);

/**
 * Creates a new symbol with the specified name and type.
 *
 * @param name The name of the symbol.
 * @param type The type of the symbol.
 * @return A pointer to the newly created symbol.
 */
symbol_ptr createNewSymbol(char* name, int type);

/**
 * Adds a st node to the st table.
 *
 * @param head A pointer to the head of the st table.
 * @param st The st node to be added.
 */
void addToStTable(st_ptr* head, st_ptr st);

/**
 * Searches for a symbol in the symbol table by name.
 *
 * @param head A pointer to the head of the symbol table.
 * @param symbolName The name of the symbol to search for.
 * @return A pointer to the found symbol, or NULL if not found.
 */
symbol_ptr searchForSymbol(symbol_ptr* head, char* symbolName);

/**
 * Initializes a st node.
 *
 * @param st The st node to be initialized.
 */
void initializeSt(st_ptr st);

/**
 * Adds an instruction word to the instruction word table.
 *
 * @param head A pointer to the head of the instruction word table.
 * @param word The instruction word to be added.
 */
void addToInsWordTable(wordIns_ptr* head, wordIns_ptr word);

/**
 * Adds a directive word to the directive word table.
 *
 * @param head A pointer to the head of the directive word table.
 * @param word The directive word to be added.
 */
void addToDirWordTable(wordDir_ptr* head, wordDir_ptr word);

/**
 * Frees the memory allocated for the st table.
 *
 * @param head The head of the st table.
 */
void freeStTable(st_ptr head);

/**
 * Frees the memory allocated for the symbol table.
 *
 * @param head The head of the symbol table.
 */
void freeSymbolTable(symbol_ptr head);

/**
 * Frees the memory allocated for the word table.
 *
 * @param head The head of the words table.
 */
void freeWordsTable(wordTable_ptr head);

/**
 * Checks if a given macro name exists in a macro table.
 *
 * @param head The head pointer of the macro table.
 * @param name The name of the macro to search for.
 * @return 1 if the macro exists in the table, 0 otherwise.
 */
int isMacro(macroPtr head, const char name[]);

/**
 * Prints the content of a macro at a specific index to a file.
 *
 * @param program The file pointer of the original program.
 * @param newFile The file pointer of the new file to write the macro content to.
 * @param macroIndex The index of the macro to print.
 * @param head The head pointer of the macro table.
 */
void printMacroToFile(FILE* program, FILE* newFile, int macroIndex, macroPtr head);

/**
 * Frees the memory occupied by the macro table.
 *
 * @param head The head pointer of the macro table.
 */
void freeMacroTable(macroPtr head);

/**
 * Frees instruction table
 *
 * @param head The head of the instruction table.
 */
void freeInsTable(wordIns_ptr head);

/**
 * Frees instruction table
 *
 * @param head The head of the directive table.
 */
void freeDirTable(wordDir_ptr head);

/**
 * Initializes a wordIns node.
 *
 * @param word The word node to be initialized.
 */
void initializeInsWord(wordIns_ptr word);

/**
 * Initializes a wordDir node.
 *
 * @param word The word node to be initialized.
 */
void initializeDirWord(wordDir_ptr word);

#endif /* TABLES_H */

