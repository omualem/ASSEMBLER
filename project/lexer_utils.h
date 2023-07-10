#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include "tables.h"

/**
 * Modifies an error flag and returns false for an error
 *
 * @param filePath The path of the file to be released
 */
#define SET_ERROR \
    (*errorFlag) = TRUE;\
    return FALSE;

/**
 * Analyzes the addressing methods for the operands in the st node.
 *
 * @param st The st node of a line.
 */
void addressingAnalyze(st_ptr st);

/**
 * Finds the first number in the given line, if there's a label, it will skip him.
 *
 * @param line The line to search for the number.
 * @param labelFlag The flag indicating whether a label is present.
 * @param st The st node for the line.
 * @return The index of the first number.
 */
int findFirstNumber(const char* line, int labelFlag, st_ptr st);

/**
 * Checks whether the given operand label is valid.
 *
 * @param label The operation label to check.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
int isValidOpLabel(const char* label);

/**
 * Checks whether the given operand number is valid.
 *
 * @param number The operation number to check.
 * @return 0 if the operation label is valid, -1 otherwise.
 */
int isValidOpNumber(char* number);

/**
 * Gets the number of operands for the specified opcode.
 *
 * @param opcode The opcode.
 * @return The number of operands.
 */
int getNumOfOperands(int opcode);

/**
 * Analyzes the addressing methods for the operands in the st node.
 *
 * @param st The st node of a line.
 */
void addressingAnalyze(st_ptr st);

/**
 * Checks whether the given label is valid.
 *
 * @param label The label to check.
 * @param symbol_head The head of the symbol table.
 * @param currentLine The current line number (for error message).
 * @param filename The name of the source file (for error message).
 * @param errorFlag A pointer to the error flag.
 * @param labelType The type of the label (Entry/External/Relocatable).
 * @return 0 if the operation label is valid, -1 otherwise.
 */
int isValidLabel(const char* label, symbol_ptr symbol_head, const char* filename, int currentLine, int* errorFlag, int labelType);

#endif /* LEXER_UTILS_H */

