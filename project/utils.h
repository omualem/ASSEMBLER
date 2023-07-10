#include "tables.h"

/**
 * Checks memory integrity after the malloc command
 *
 * @param var The tested variable
 */
#define MALLOC_CHECK(var) \
    if (var == NULL){ \
    printf("cannot allocated memory\n"); \
    return NULL;}

/**
 * Checks file integrity after the fopen command
 *
 * @param file The file address
 */

#define FILE_CHECK(file) \
    if (file == NULL){ \
    printf("Cannot open file\n"); \
    return NULL;}

/**
 * Safely frees the allocated memory
 *
 * @param filePath The path of the file to be released
 */
#define SAFE_FREE(filePath) if ((filePath) != NULL) { free(filePath);}

/**
 * Checks whether the given label is a register.
 *
 * @param label The label to check.
 * @return 0-7 if the label is a register label (returns register number), 8 otherwise.
 */
int isRegisterLabel(const char* label);

/**
 * Checks whether the given label is an instruction label.
 *
 * @param label The label to check.
 * @return 0-15 if the label is an instruction label (returns instruction number), 16 otherwise.
 */
int isInstructionLabel(const char* label);

/**
 * Checks whether the given label is a directive label.
 *
 * @param label The label to check.
 * @return 0-3 if the label is a directive label (returns directive number), 4 otherwise.
 */
int isDirectiveLabel(const char* label);

/**
 * Skips white characters (spaces, tabs) in the given line starting from the specified index.
 * Updates the index to the next non-white character or the end of the line.
 *
 * @param line The line to skip white characters from.
 * @param index A pointer to the index from which to start skipping white characters.
 * @return The number of characters skipped.
 */
int skipWhiteChars(const char* line, int* index);

/**
 * Finds the index of the first occurrence of the specified character in the given line.
 *
 * @param line The line to search for the character.
 * @param ch The character to search for.
 * @return The index of the first occurrence of the character.
 */
int findIndexFromLine(const char* line, char ch);

/**
 * Checks whether the specified file exists.
 *
 * @param filename The name of the file to check.
 * @return 0 if the file exists, -1 otherwise.
 */
int fileExists(const char* filename);

/**
 * Sets the output file name by appending the specified extension to the base file name.
 *
 * @param file The base file name.
 * @param ext The extension to append.
 * @return A pointer to the modified output file name.
 */
char* setOutputFile(const char* file, char* ext);

/**
 * Decodes a Base64 character.
 *
 * @param c The char to decode.
 * @return The decoded char.
 */
char base64DecodeChar(char c);

/**
 * Converts a binary number to its decimal representation.
 *
 * @param binArray The array containing the binary number.
 * @return The decimal representation of the binary number.
 */
int binaryToDecimal(const int binArray[]);

/**
 * Prints two Base64 characters to the specified output file.
 *
 * @param c1 The first Base64 character.
 * @param c2 The second Base64 character.
 * @param obFile The output file pointer.
 */
void printBase64ToFile(char c1, char c2, FILE* obFile);

/**
 * Converts a binary code to Base64 characters and writes them to the specified output file.
 *
 * @param binCode The array containing the binary code.
 * @param obFile The output file pointer.
 */
void convertToCharsBase64(int binCode[], FILE* obFile);

/**
 * Converts a decimal number to a binary representation.
 *
 * @param arr The array to store the binary representation.
 * @param number The decimal number to convert.
 * @param howManyBits The number of bits to represent the binary number.
 */
void decimalToBinary(int arr[], int number, int howManyBits);

/**
 * Converts a character to its binary representation.
 *
 * @param ch The character to convert.
 * @param charArr The array to store the binary representation of the character.
 */
void charToBinary(char ch, int charArr[]);

/**
 * Resets the arrays used for storing instruction information.
 *
 * @param sourceAdrArr The array representing the source addressing method.
 * @param destAdrArr The array representing the destination addressing method.
 * @param opcodeArr The array representing the opcode.
 * @param areArr The array representing the ARE (Absolute/Relative/External) attribute.
 * @param numInsArr The array representing the numeric instruction.
 * @param srcReg The array representing the source register.
 * @param desReg The array representing the destination register.
 */
void resetInsArr(int sourceAdrArr[], int destAdrArr[], int opcodeArr[], int areArr[], int numInsArr[], int srcReg[], int desReg[]);
