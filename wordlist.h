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

/* this is a pointer to a memery word list */
typedef struct memnode* mempointer;
/* this is a memory word node */
typedef struct memnode {
    int memword[BITS_OF_WORD];
    int dc;
    mempointer next;
}wnode;

/* sets all the slots in memword to 0 */
void initialize_memword(int* memword);

/* prints the memword into the .ob file
 * memword is the memory word
 * obfile is the .ob file
 * IC is the instruction counter */
void print_word(int* memword,FILE** obfile,int* IC);