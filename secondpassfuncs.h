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

/* this function fills the .ent file with entry labels
 * head is the head of the symbol list
 * entfile is the .ent file */
void fill_entry(symnode* head,FILE** entfile);

/* sets all the slots in memword to 0 */
void initialize_memword(int* memword);

/* word is a string from the file
 * the function checks if word is one of the commands
 * if it is the function returns 0
 * otherwise returns 1 */
int whataction(char* word);

/* this function gets the binary number of the command and puts it into memword
 * memword represents a memory word
 * command is the given command */
void code_command(int* memword,int command);

/* this function is used when getting an action the needs 2 operands.
 * the function will check what is the operand type and after that it will code and print it to the .ob file.
 * if an external label was found it will print it to the .ext file.
 * word is the operand
 * symhead is the head of the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the instruction counter
 * memword is the memory word
 * obfile is the .ob file
 * ext file is the .ext file
 * returns -1 on error
 * otherwise returns the number that corresponds with the operand type */
int code_operand2(char* word,symnode* symhead ,char* filename, int counter,int* IC,int* memword,FILE** obfile,FILE** extfile);

/* this function is used when there is only one operand to code and print.
 * the function checks what is the operand and codes it accordingly. then, it is printed.
 * the function does not code jump operands with parameters.
 * word is the operand
 * symhead is the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the instruction counter
 * memword represents a memory word
 * obfile is the .ob file
 * extfile is the .ext file
 * errros is a flag that tells the function to skip some tests
 * returns -1 on error
 * otherwise returns the number that corresponds with the operand type */
int code_operand(char *word, symnode *symhead, char *filename, int counter, int *IC, int *memword, FILE **obfile,
                 FILE **extfile);

/* this function codes and prints the jump operand with parameters.
 * word is the whole operand
 * head is the head of the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the instruction counter
 * memword is the memory word
 * obfile is the .ob file
 * extfile is the .ext file
 * returns 1 on error
 * otherwise returns 0 */
int code_jump(char* word,symnode* head, char* filename,int counter,int* IC, int* memword,FILE** obfile,FILE** extfile);


/* this functions checks what is the operand type
 * word holds the string to be checked
 * symhead points to the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the command counter
 * returns a number that corresponds to the operand type
 * otherwise returns -1 */
int check_operand(char *word, char *filename, int counter, int *IC);

/* this function copies two operands into op1 and op2
 * word holds both operand seperated with ,
 * filename is the file name
 * counter is the number of the line we are in
 * op1 will hold the first operand
 * op2 will hold the second operand
 * if errors were found returns 1
 * otherwise returns 0 */
int get_ops(char* word,char* filename,int counter,char* op1,char* op2);

/* this function codes the operand types into memword
 * memword is the memory word
 * op1 is the first operand type
 * op2 is the second operand type */
void code_2_op_type(int* memword,int op1,int op2);

/* prints the memword into the .ob file
 * memword is the memory word
 * obfile is the .ob file
 * IC is the instruction counter */
void print_word(int* memword,FILE** obfile,int* IC);

/* this function is used when 2 registers are the operands.
 * it combines them into memword wo be printed to the .ob file.
 * par1 is the first register
 * par2 is the second register
 * IC is the instruction counter
 * obfile is the .ob file */
void code_two_regs(char* par1, char* par2,int* IC,FILE** obfile);

/* prints the source register.
 * regnum is the char that represents the number of the register
 * obfile is the .ob file
 * IC is the instruction counter */
void print_left_reg(char regnum,FILE** obfile,int* IC);

/* this function deletes the .ob .ext .ent files when errors where found
 * filename is the file name
 * obfile is the .ob file
 * entfile is the .ent file
 * extfile is the .ext file */
void delete_files(char* filename, FILE** obfile, FILE** entfile, FILE** extfile);

/* this function goes over the data line.
 * each number it finds gets inserted into the memory list to be printed later
 * DC is the data counter
 * memlist is the head of the memory list
 * returns 1 on error
 * otherwise returns 0 */
int code_data(int *DC,mempointer* memlist);

/* this function takes the string from the .am file.
 * for each char it encounters, it will crate a memory node and store its value inside
 * returns 0 */
int code_string(int* DC,mempointer* head);

/* this function checks that all the external labels were used in the code
 * head is the head of the symbol list
 * filename is the file name
 * returns 1 on error
 * otherwise returns 0 */
int check_extern_nodes(symnode* head,char* filename);

/* this function frees the memory list */
void free_word_list(mempointer* head);

/* this function goes over the memory list and prints node by node their values to the .ob file */
void print_data(mempointer* head,int* IC, FILE** obfile);

/* this function checks if the files are empty and deletes them if they are
 * filename is the filename
 * entfile is the .ent file
 * extfile is the .ext file*/
void delete_empty(char* filename, FILE** entfile, FILE** extfile);

/* prints the binary representation of a destination register
 * num is the number to be printed in binary
 * obfile is the .ob file
 * IC is the instruction counter */
void print_one_reg(int num, FILE** obfile,int* IC);

/* this function prints the binary representation of a decimal number.
 * num points to the string that represents the number
 * IC is the instruction counter
 * obfile is the .ob file
 * memword is the memory word */
void print_num(char* num,int* IC, FILE** obfile,int* memword);

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

/* this function prints the address of a label
 * num is the address to be printed
 * IC is the instruction counter
 * obfile is the .ob file
 * memword is the memory word */
void print_address(int num,int* IC,FILE** obfile,int* memword);

/* this function is used in check_jump to check if the parameter are valid
 * word is the parameter to be checked
 * filename is the file name
 * counter is the number of line we are in
 * regnum will hold the number of registers that were detected as parameters in the line
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_parameter(char* word,char* filename,int counter,int* regnum);

/* codes the parameters into memword and then memword is printed to the .ob file
 * memword is the memory word
 * IC is the instruction counter
 * obfile is the .ob file
 * par1type is the number that corresponds with the first operand type
 * par2type is the number that corresponds with the second operand type */
void code_par_type(int* memword,int* IC, FILE** obfile, int par1type,int par2type);

/* this function prints the binary representation of a destination register
 * num is a char that represent the register's number
 * obfile is the .ob file
 * IC is the instruction counter */
void print_1_reg(char num, FILE** obfile,int* IC);



