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

/* using this struct we will create a table that tells us what addressing type is valid for each command */
typedef struct action_options{
    int source[4];
    int dest[4];
}aop;

/* IC is the instruction counter
 * DC is the data counter
 * source is the .am file
 * symhead points to the label list
 * tpointer points to the table of valid addressing types
 * filename is the filename
 * this function oversees the first pass of the assembler
 * returns 1 if an error was found
 * otherwise returns 0*/
int firstpass(int* IC, int*DC,FILE** source,symnode* symhead, aop** tpointer, char* filename);

/* this function checks if the files exists and are not empty.
 * filenames is a pointer to the filename.
 * tempfd is a pointer to a FILE structure.
 * if a file doesn't exist or is empty then we return 1.
 * otherwise we return 0 */
int check_files(char* filenames, FILE** tempfd);

/* this function finds and spreads the macros
 * tempfd is the source file
 * filename is the file name of the source file without the extension
 * at the end of the function newfile will point to the file with the spread macros
 * if an error occurred then return 1
 * otherwise return 0 */
int preassembler(FILE** tempfd, char* filename, FILE** newfile);

/* this function oversees the second pass the of assembler.
 * it goes over the .am file and prints the machine code. and fills the .ent and .ext files.
 * IC is the instruction counter
 * DC is the data counter
 * source is the .am file
 * symhead points to the symbol list
 * filename is the filename
 * prv_errors lets us know if there were errors from firstpass
 * returns 1 on error
 * otherwise returns 0 */
int secondpass(int* IC, int* DC, FILE** source,symnode* symhead,char* filename,int prv_errors);

/* if there were errors in firstpass then this function is called.
 * it looks for labels that were not defined*/
int error_check(int* IC, int* DC,FILE** source,symnode* symhead, char* filename);


/* this function frees the symbol list
 * head is the head of the list */
void free_symlist(symnode* head);