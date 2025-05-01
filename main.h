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

/* using this struct we will create a table that tells us what addressing type is valid for each command */
typedef struct action_options{
    int source[4];
    int dest[4];
}aop;


/* this is a pointer to a macro list node */
typedef struct macronode* mpointer;
/* this is a macro list node */
typedef struct macronode {
    char* macroname;
    char* macro;
    mpointer next;
} mnode;

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

/* head is the pointer to the pointer of the first link in the macro list
 * the function frees the list */
void free_macro_list(mpointer* head);

/* head is the pointer to the pointer of the first link in the macro list
 * mname is the macro name to be added
 * mac is the macro content to be added
 * the function adds another node to the list which contains the macro name and the macro itself
 * returns 0 if all went well
 * otherwise returns 1*/
int add_mnode(mpointer* head, char* mname, char* mac);

/* tempfd points to the source file
 * head points to the first link in the macro list
 * the function goes over the source file and looks for macro definitions
 * when it finds one it adds it to a list
 * if an error occurs returns 1
 * otherwise returns 0 */
int find_macros(FILE** tempfd,mpointer* head,char* filename);

/* mname is the macro name
 * the function checks if the macro name is not a reserved word
 * it the macro name is a reserved word then 1 is returned
 * otherwise 0 is returned */
int check_macro_name(char* mname);

/* tempfd points to the source file
 * mdata will hold the macro contents
 * head points to the first link of the macro list
 * counter holds the number of the link that is being read in the source file
 * once a macro definition was found, this function is called
 * the function copies the contents of the macro to mdata
 * if an error occurred it returns 1
 * otherwise returns 0 */
int get_macro(FILE **tempfd, char **mdata, int *counter, char *filename);

/* source is the source file
 * newfile will point to the file with spread macros
 * filename is the name of the file without the extension
 * head points to the first link in the macro list
 * the function creates a new file with the contents of the source file
 * with spread macros.
 * returns 1 on error
 * otherwise returns 0 */
int spread_macro(FILE** source,FILE** newfile,char* filename,mpointer* head);

/* macro is the macro to be put in newfile
 * the function puts the macro in newfile */
void put_macro(char* macro,FILE** newfile);



/* word is a word from the line
 * counter is the number of the line we are in
 * filename is the filename
 * errors if a flag that signals if any errors were found
 * the function checks if word is a label
 * returns 1 if it is not a label
 * otherwise returns 0*/
int islabel(char* word,int counter,char* filename, int* errors);

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

/* head points to head of the IC or DC list
 * name is the label name
 * curnode is a pointer to a symnode node
 * the function adds a node to the list
 * returns 1 if an error occurred
 * otherwise returns 0*/
int add_symnode(symnode* head,char* name,symnode* curnode);

/* tpointer points to the table of addressing types
 * the functions fills the table */
void initialize_action_table(aop** tpointer);

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

/* this functions checks what is the operand type
 * word holds the string to be checked
 * symhead points to the symbol list
 * filename is the file name
 * counter is the number of the line we are in
 * IC is the command counter
 * returns a number that corresponds to the operand type
 * otherwise returns -1 */
int check_operand(char *word, char *filename, int counter, int *IC);

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

/* this function deletes the newly added node */
void delete_cur_node(symnode* head, symnode* cur);

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

/* this function is used in check_jump to check if the parameter are valid
 * word is the parameter to be checked
 * filename is the file name
 * counter is the number of line we are in
 * regnum will hold the number of registers that were detected as parameters in the line
 * returns 1 if errors were found
 * otherwise returns 0 */
int check_parameter(char* word,char* filename,int counter,int* regnum);

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

/* checks if word is a number
 * filename is the file name
 * counter is the number of the line we are in
 * returns 1 if word is not a number
 * otherwise returns 0 */
int isnum2(char* word,char* filename,int counter);

/* this function checks if word is a label
 * counter is the number of the line we are in
 * filename is the file name
 * errors will be updated if any errors were found
 * returns 1 if word is not a label
 * otherwise returns 0 */
int islabel2(char** word,int counter,char* filename, int* errors);

/* this function looks for the label in the symbol list
 * and points to it with curnode
 * name is the label name */
void findnode(symnode* head, symnode* curnode,char* name);

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


/* this function frees the symbol list
 * head is the head of the list */
void free_symlist(symnode* head);




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

/* this function checks that all the external labels were used in the code
 * head is the head of the symbol list
 * filename is the file name
 * returns 1 on error
 * otherwise returns 0 */
int check_extern_nodes(symnode* head,char* filename);

/* sets all the slots in memword to 0 */
void initialize_memword(int* memword);

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

/* this function gets the binary number of the command and puts it into memword
 * memword represents a memory word
 * command is the given command */
void code_command(int* memword,int command);

/* prints the memword into the .ob file
 * memword is the memory word
 * obfile is the .ob file
 * IC is the instruction counter */
void print_word(int* memword,FILE** obfile,int* IC);

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

/* codes the parameters into memword and then memword is printed to the .ob file
 * memword is the memory word
 * IC is the instruction counter
 * obfile is the .ob file
 * par1type is the number that corresponds with the first operand type
 * par2type is the number that corresponds with the second operand type */
void code_par_type(int* memword,int* IC, FILE** obfile, int par1type,int par2type);

/* this function gets a number and puts the binary number into the array "binary"
 * binary is the array that will hold the binary number at the end of the function
 * num is the number to be converted to binary */
void get_bin(int* binary,int num);

/* this function is used when 2 registers are the operands.
 * it combines them into memword wo be printed to the .ob file.
 * par1 is the first register
 * par2 is the second register
 * IC is the instruction counter
 * obfile is the .ob file */
void code_two_regs(char* par1, char* par2,int* IC,FILE** obfile);

/* this function prints the binary representation of a destination register
 * num is a char that represent the register's number
 * obfile is the .ob file
 * IC is the instruction counter */
void print_1_reg(char num, FILE** obfile,int* IC);

/* this function codes the operand types into memword
 * memword is the memory word
 * op1 is the first operand type
 * op2 is the second operand type */
void code_2_op_type(int* memword,int op1,int op2);

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

/* this function checks if the files are empty and deletes them if they are
 * filename is the filename
 * entfile is the .ent file
 * extfile is the .ext file*/
void delete_empty(char* filename, FILE** entfile, FILE** extfile);

/* this function checks if the files are empty.
 * it also closes the files
 * file is the pointer to the file
 * returns 1 if the file is empty
 * otherwise returns 0 */
int isempty(FILE** file);

/* adds a memory word node to the list
 * head is the head of the list
 * DC is the data counter
 * curnode is a pointer that will get the address of the newly added node
 * returns 1 on error
 * otherwise returns 0 */
int add_word_node(mempointer* head,int* DC,mempointer* curnode);

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

/* this function goes over the memory list and prints node by node their values to the .ob file */
void print_data(mempointer* head,int* IC, FILE** obfile);

/* this function frees the memory list */
void free_word_list(mempointer* head);










/* the function checks if a label was not used */
int islabel_no_errors(char *word, int *errors);


/* the function checks if a label was not used in a jump parameter*/
int check_jump_no_errors(char* word, int* IC,char* filename,int counter,symnode* symhead,FILE** extfile);


/* get_ops copies the operands into op1 and op2
 * the function does not report errors*/
int get_ops_no_errors(char *word, char *op1, char *op2);

/* the function compares word with the label name from the label list
 * the function does not report errors*/
int check_lists_no_errors(char *word, symnode *symname, int *modify);

/* this function checks if there is a comma that separates the operands
 * the function does not report errors*/
int check_commas_no_errors(char *word);

/* if there were errors in firstpass then this function is called.
 * it looks for labels that were not defined*/
int error_check(int* IC, int* DC,FILE** source,symnode* symhead, char* filename);