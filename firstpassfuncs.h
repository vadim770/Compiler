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

/* tpointer points to the table of addressing types
 * the functions fills the table */
void initialize_action_table(aop** tpointer);

/* word is a word from the line
 * counter is the number of the line we are in
 * filename is the filename
 * errors if a flag that signals if any errors were found
 * the function checks if word is a label
 * returns 1 if it is not a label
 * otherwise returns 0*/
int islabel(char* word,int counter,char* filename, int* errors);

/* word is a string from the file
 * the function checks that word is not a reserved word
 * if word is a reserved word it returns 1
 * otherwise returns 0*/
int check_name(char* word);

/* word is a string from the file
 * symhead is the head of the label list
 * filename is the file name
 * counter is the number of line we are in
 * modify will be changed to 1 if the label was already defined 0 otherwise
 * the function checks if there is any invalid label definitions
 * if there were than we return 1
 * otherwise returns 0 */
int check_lists(char* word, symnode* symname,char* filename, int counter,int* modify);

/* word is a string from the file
 * the function checks if word is one of the commands
 * if it is the function returns 0
 * otherwise returns 1 */
int whataction(char* word);

/* head points to head of the IC or DC list
 * name is the label name
 * curnode is a pointer to a symnode node
 * the function adds a node to the list
 * returns 1 if an error occurred
 * otherwise returns 0*/
int add_symnode(symnode* head,char* name,symnode* curnode);

/* this function looks for the label in the symbol list
 * and points to it with curnode
 * name is the label name */
void findnode(symnode* head, symnode* curnode,char* name);

/* this functions checks what is the operand type
 * word holds the string to be checked
 * symhead points to the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the command counter
 * returns a number that corresponds to the operand type
 * otherwise returns -1 */
int check_operand(char *word, char *filename, int counter, int *IC);

/* this function checks the number of commas in word
 * word is the string to be checked
 * filename is the file name
 * counter is the number of the line we are in
 * returns 0 if there is one comma
 * otherwise returns 1 */
int check_commas(char* word,char* filename,int counter);

/* this function copies two operands into op1 and op2
 * word holds both operand seperated with ,
 * filename is the file name
 * counter is the number of the line we are in
 * op1 will hold the first operand
 * op2 will hold the second operand
 * if errors were found returns 1
 * otherwise returns 0 */
int get_ops(char* word,char* filename,int counter,char* op1,char* op2);

/* this function checks if extern and entry directions are valid
 * word holds the string after the direction
 * counter is the number of line we are in
 * filename is the file name
 * symhead is a pointer to the symbol list
 * command holds the number that corresponds to entry and extern
 * returns 1 if error was found
 * returns -1 if there was memory failure
 * returns 0 otherwise */
int analyze_dir(char* word,int counter, char* filename, symnode* symhead,int command);

/* this function checks the rest of the line after .data direction and updates the data counter accordingly
 * filename is the file name
 * counter is the number of the line we are in
 * DC is the data counter
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_data(char* filename,int counter,int* DC);

/* this function checks the rest of the line after .string direction and updates the  data counter accordingly
 * filename is the file name
 * counter is the number of the line we are in
 * DC is the data counter
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_string(char* filename,int counter, int* DC);

/* this function goes over the symbol list and updates the DC's
 * head is the head of the list
 * IC points to the instruction counter */
void updatedc(symnode* head,int* IC);

/* this function checks that labels that were declared as entry were defined in the file
 * head is the head of the symbol list
 * filename is the filename
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_entry_exits(symnode* head,char* filename);

/* this function checks that the extern direction is without errors
 * word is the string to be checked
 * symhead is a pointer to the symbol list
 * filename is the file name
 * counter is the number of line we are in
 * returns 1 if error was found
 * returns -1 is there was memory failure
 * returns 0 otherwise */
int check_extern(char* word, symnode* symhead,char* filename, int counter);

/* this function checks that the entry direction is without errors
 * word is the string to be checked
 * symhead is a pointer to the symbol list
 * filename is the file name
 * counter is the number of line we are in
 * returns 1 if error was found
 * returns -1 is there was memory failure
 * returns 0 otherwise */
int check_entry(char* word, symnode* symhead,char* filename, int counter);

/* this function checks that the jump operand is valid
 * word is the operand to be checked
 * IC is the command counter
 * filename is the file name
 * counter is the number of line we are in
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_jump(char* word, int* IC,char* filename,int counter);

/* checks if word is a register
 * returns 0 if it is
 * otherwise returns 1 */
int isregister(char* word);

/* this functions checks if word is a valid number
 * word is the string to be chekced
 * filename is the file name
 * counter is the number of the line we are in
 * returns -1 if errors were found
 * returns 1 if it is not a number
 * if it is a number than return 0 */
int isnum(char* word,char* filename,int counter);

/* this function checks if word is a label
 * counter is the number of the line we are in
 * filename is the file name
 * errors will be updated if any errors were found
 * returns 1 if word is not a label
 * otherwise returns 0 */
int islabel2(char** word,int counter,char* filename, int* errors);

/* this function is used in check_jump to check if the parameter are valid
 * word is the parameter to be checked
 * filename is the file name
 * counter is the number of line we are in
 * regnum will hold the number of registers that were detected as parameters in the line
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_parameter(char* word,char* filename,int counter,int* regnum);



