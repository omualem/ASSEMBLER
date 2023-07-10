#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "preprocess.h"
#include "secondPass.h"
#include "utils.h"
#include "globals.h"


void decodeFile(char* file){
    char* asFileName;
    char* amFileName;
    st_ptr st_head = NULL;
    symbol_ptr symbol_head  =  NULL;
    wordTable_ptr wordTable_head  =  NULL;
    asFileName = setOutputFile(file,".as");

    if(fileExists(asFileName)==TRUE){

        /*pre process on as file  and creat am file*/
        amFileName = preProcessor(asFileName,file);

        /*analyzing the whole am file, if there is an error, it returns NULL*/
        st_head = lexer(amFileName);
        symbol_head  = (st_head!=NULL)?st_head->symbol_head:NULL;

        /*Performs the first of 2 passes (even if there was an error, it will skip everything).*/
        wordTable_head = firstPass(st_head,symbol_head,amFileName);

        /*Performs the second of 2 passes*/
        secondPass(symbol_head,wordTable_head,file);

        /*frees the allocated memory that created*/
        SAFE_FREE(amFileName)
        SAFE_FREE(asFileName)
        freeStTable(st_head);
        freeSymbolTable(symbol_head);
        freeWordsTable(wordTable_head);
    }
    else{
        printf("ERROR: the file %s doesn't exist\n",file);
        return;
    }
}
