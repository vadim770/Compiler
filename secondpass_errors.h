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

/* this function creates the .ob .ent .ext files
 * it also prints the number of IC and DC lines at that start of the .ob file
 * obfile is the .ob file
 * entfile is the .ent file
 * ext file is the .ext file
 * filename is the file name
 * IC is the instruction counter
 * DC is the data counter
 * returns 1 on error
 * otherwise returns 0 */
int open_files(FILE** obfile,FILE** entfile, FILE** extfile,char* filename,int* IC, int* DC);

/* the function checks if a label was not used */
int islabel_no_errors(char *word, int *errors);

/* word is a string from the file
 * the function checks that word is not a reserved word
 * if word is a reserved word it returns 1
 * otherwise returns 0*/
int check_name(char* word);

/* the function compares word with the label name from the label list
 * the function does not report errors*/
int check_lists_no_errors(char *word, symnode *symname, int *modify);

/* word is a string from the file
 * the function checks if word is one of the commands
 * if it is the function returns 0
 * otherwise returns 1 */
int whataction(char* word);

/* the function checks if a label was not used in a jump parameter*/
int check_jump_no_errors(char* word, int* IC,char* filename,int counter,symnode* symhead,FILE** extfile);

/* this function was called if an operand is a label.
 * the function checks if the label was defined and if it is external or internal.
 * head is the head of the symbol list
 * word is the label name
 * counter is the number of the line we are in
 * filename is the file name
 * extfile is the .ext file
 * IC is the instruction counter
 * returns -1 if label is external
 * returns -2 on error
 * otherwise returns the IC address */
int get_address(symnode* head,char* word,int counter, char* filename,FILE** extfile,int* IC);

/* this function checks if there is a comma that separates the operands
 * the function does not report errors*/
int check_commas_no_errors(char *word);

/* get_ops copies the operands into op1 and op2
 * the function does not report errors*/
int get_ops_no_errors(char *word, char *op1, char *op2);

/* this function checks that all the external labels were used in the code
 * head is the head of the symbol list
 * filename is the file name
 * returns 1 on error
 * otherwise returns 0 */
int check_extern_nodes(symnode* head,char* filename);

/* this function deletes the .ob .ext .ent files when errors where found
 * filename is the file name
 * obfile is the .ob file
 * entfile is the .ent file
 * extfile is the .ext file */
void delete_files(char* filename, FILE** obfile, FILE** entfile, FILE** extfile);