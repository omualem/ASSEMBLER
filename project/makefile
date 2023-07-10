assembler: assembler.o preprocess.o lexer.o tables.o utils.o decode.o firstPass.o secondPass.o lexer_utils.o
	gcc -g -Wall -ansi -pedantic assembler.o preprocess.o lexer.o lexer_utils.o tables.o utils.o decode.o firstPass.o secondPass.o -o assembler

assembler.o:  assembler.c  decode.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocess.o:  preprocess.c  tables.h globals.h preprocess.h utils.h
	gcc -c -Wall -ansi -pedantic preprocess.c -o preprocess.o

lexer.o:  lexer.c lexer.h globals.h preprocess.h utils.h lexer_utils.h
	gcc -c -Wall -ansi -pedantic lexer.c -o lexer.o

lexer_utils.o:  lexer_utils.c lexer_utils.h globals.h utils.h
	gcc -c -Wall -ansi -pedantic lexer_utils.c -o lexer_utils.o

tables.o:  tables.c tables.h globals.h
	gcc -c -Wall -ansi -pedantic tables.c -o tables.o

utils.o:  utils.c utils.h globals.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o

decode.o:  decode.c decode.h preprocess.h secondPass.h utils.h globals.h
	gcc -c -Wall -ansi -pedantic decode.c -o decode.o

firstPass.o:  firstPass.c firstPass.h globals.h utils.o
	gcc -c -Wall -ansi -pedantic firstPass.c -o firstPass.o

secondPass.o:  secondPass.c secondPass.h globals.h utils.h
	gcc -c -Wall -ansi -pedantic secondPass.c -o secondPass.o

