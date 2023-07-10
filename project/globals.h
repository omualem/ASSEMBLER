#define CP_MEMORY 1024  /*The size of the computer's memory*/
#define ADDRESS_START 100   /*The starting address of the words*/

/*The maximum and minimum number that can be represented in 12 bits (signed)*/
#define MAX_VALID_DIR_NUMBER 2047
#define MIN_VALID_DIR_NUMBER -2048

/*The maximum and minimum number that can be represented in 10 bits (signed)*/
#define MAX_VALID_INS_NUMBER 511
#define MIN_VALID_INS_NUMBER -512

/*Some definition of true and false (since some tests will return an enum)*/
#define TRUE 0
#define FALSE (-1)

/*A definition for the representation of a character*/
#define SPACE_BAR ' '
#define COMMENT ';'
#define TAB '\t'
#define END_OF_LINE '\n'
#define COMMA ','
#define ZERO_NUMBER '0'
#define APOSTROPHES '"'
#define MINUS '-'
#define PLUS '+'
#define NULL_TERM '\0'

/**
 * Enumeration of registers.
 */
enum registers{r0,r1,r2,r3,r4,r5,r6,r7,non_reg};

/**
 * Enumeration of operation codes.
 */
enum op_codes{mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop,non_op};

/**
 * Enumeration of encoding types.
 */
enum encode_type {
    external,    /* External encoding  */
    relocatable, /* Relocatable encoding */
    entry        /* Entry encoding */
};

/**
 * Enumeration of addressing methods.
 */
enum addressing_methods {
    immediate = 1,   /* Immediate addressing method */
    direct = 3,      /* Direct addressing method */
    reg_direct = 5   /* Register direct addressing method */
};

/**
 * Enumeration of directives.
 */
enum directives {
    DATA,        /* Data directive */
    STRING,      /* String directive */
    ENTRY,       /* Entry directive */
    EXTERN,      /* Extern directive */
    non_dir      /* Not a directive */
};

/**
 * Enumeration of sentence types.
 */
enum sentenceType {
    directive,   /* Directive sentence type */
    instruction  /* Instruction sentence type */
};

/**
 * Enumeration of operand types.
 */
enum operandType {
    number,      /* Number operand type */
    label,       /* Label operand type */
    reg          /* Register operand type */
};

/**
 * Enumeration of operand methods.
 */
enum operandMethod {
    source,      /* Source operand method */
    destination  /* Destination operand method */
};


