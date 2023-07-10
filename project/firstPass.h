#include "lexer.h"

/**
 * Macro for checking if there is enough additional memory to execute a command.
 * If the condition is not satisfied, an error message is printed and NULL is returned.
 */
#define MEM_CHECK \
    if(DC+IC+ADDRESS_START>CP_MEMORY){ \
    printf("Error: There is not enough additional memory to execute the command in %s\n",outputName); \
    errorFlag=TRUE;\
    break;}

/**
 * Performs the first pass of a two-pass assembler, generating a word table.
 *
 * @param st_head The head pointer of the sentenceTree table.
 * @param symbol_head The head pointer of the symbol table.
 * @param outputName The name of the output file.
 * @return A pointer to the generated word table.
 */
wordTable_ptr firstPass(st_ptr st_head, symbol_ptr symbol_head, char* outputName);



