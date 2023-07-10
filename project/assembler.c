#include <stdio.h>
#include "decode.h"

int main(int argc, char *argv[]) {
    int i;
    if (argc < 2)
        printf("No file names provided.\n");
    for (i = 1; i < argc; i++)
        decodeFile(argv[i]);
    return 1;
}