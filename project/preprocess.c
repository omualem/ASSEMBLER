#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tables.h"
#include "utils.h"
#include "preprocess.h"
#include "globals.h"

char* preProcessor(char* asFileName,char* originFile){

    char* amFilename = setOutputFile(originFile,".am");
    FILE* asFile = fopen(asFileName, "r");
    FILE* amFile = fopen(amFilename, "w");
    char* delim = " \t\n";
    char* line;
    char* lineCopy;
    int currentLine=1,mcrFlag=FALSE,macroIndex;
    macroPtr head = NULL,lastMcr= NULL;
    long originalPosition;

    lineCopy = (char *) malloc(MAX_LENGTH_LINE_EXTENDED);
    line = (char *) malloc(MAX_LENGTH_LINE_EXTENDED);

    FILE_CHECK(asFile)
    MALLOC_CHECK(line)
    MALLOC_CHECK(lineCopy)


    while (fgets(line, MAX_LENGTH_LINE_EXTENDED, asFile)) {
        char* command;
        strcpy(lineCopy,line);
        command = strtok(line, delim);

        /*Skips a blank line and a comment line*/
        if(command!=NULL && command[0]!=COMMENT){

            /*Checks whether a macro is in the definition*/
            macroIndex = isMacro(head,command);
            if (macroIndex != FALSE) {
                originalPosition = ftell(asFile);
                printMacroToFile(asFile,amFile,macroIndex,head);
                fseek(asFile, originalPosition, SEEK_SET);
            }

            /*Checks whether the line starts with a macro definition*/
            if (strcmp(command, "mcro") == TRUE) {
                char *mcrName;
                macroPtr temp;

                mcrName = (char *) malloc(*command + 1);
                temp = (macroPtr) malloc(sizeof(macro));

                MALLOC_CHECK(mcrName)
                MALLOC_CHECK(temp)

                command = strtok(NULL, delim);

                if(isInstructionLabel(command) != non_op || isDirectiveLabel(command) != non_dir || isRegisterLabel(command) != non_reg){
                    printf("Error: Macro name cannot be Instruction/Directive/Register name in file %s.as\n",originFile);
                    remove(amFilename);
                    SAFE_FREE(amFilename)
                    return NULL;
                }

                else if(command==NULL){
                    printf("Error: Macro name is not defined in file %s.as\n",originFile);
                    remove(amFilename);
                    SAFE_FREE(amFilename)
                    return NULL;
                }

                /*Creates a macro link with a starting line*/
                mcrFlag=TRUE;
                strcpy(mcrName, command);
                temp->name = mcrName;
                temp->startLine = currentLine + 1;
                temp->next=NULL;

                if (head == NULL){
                    head = temp;
                    lastMcr = head;
                }
                else {
                    lastMcr->next = temp;
                    lastMcr = temp;
                }

                command = strtok(NULL, delim);

                if(command!=NULL){
                    printf("Error: Extraneous text after end of macro definition in file %s.as\n",originFile);
                    remove(amFilename);
                    SAFE_FREE(amFilename)
                    return NULL;
                }

            }

            /*If the line is a closing macro*/
            else if (strcmp(command, "endmcro") == TRUE) {
                lastMcr->endingLine = currentLine-1;
                mcrFlag=FALSE;
            }

            /*If the line is a line without a macro definition*/
            else if(mcrFlag==FALSE  && macroIndex == FALSE){
                fprintf(amFile, "%s", lineCopy);
            }
        }
        currentLine++;
    }

    /*Frees all allocated memory*/
    free(lineCopy);
    free(line);
    fclose(amFile);
    fclose(asFile);
    freeMacroTable(head);

    /*if the file is empty*/
    if(currentLine==1){
        remove(amFilename);
        return NULL;
    }

    /*name of the created am file*/
    return amFilename;
}


