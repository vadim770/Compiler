#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 82 /* 82 is the max length of a line */
#define MAX_SYM 31 /* 31 is the max symbol name */
#define MAX_FILENAME 300 /* this is the max file name */
#define NUM_OF_ACTIONS 16 /* this is the number of the action commands */
#define BITS_OF_WORD 14 /* the number of bits that a memory word can hold */
#define ILLEGAL_NAMES 31 /* the number of names that a macro or a label cant have */

enum action {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP,DATA,STRING,ENTRY,EXTERN}; /* this represents the commands */
enum addressing {IMMEDIATE,DIRECT,JUMP,REGISTER}; /* this represents the addressing types */

/* this is a pointer to a symbol list node */
typedef struct symbolnode* symnode;
/* this is a symbol list node */
typedef struct symbolnode {
    char symname[MAX_SYM]; /* contains the symbol name */
    int isic; /* signals if it is an action word */
    int isdc; /* signals if it is a direction word */
    int isinternal; /* signals that the label was defined in the file */
    int isextern; /* signals if it is external */
    int isentry; /* signals if the label could be used in another file */
    int icadrs;    /* the ic address of the word */
    int dcadrs;   /* the dc address of the word */
    int found; /* relevant on nodes that are extern. used to check if these labels were used after being declared */
    symnode next; /* points to the next node */
} snode;