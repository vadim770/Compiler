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

/* this is a pointer to a macro list node */
typedef struct macronode* mpointer;
/* this is a macro list node */
typedef struct macronode {
    char* macroname;
    char* macro;
    mpointer next;
} mnode;

/* tempfd points to the source file
 * head points to the first link in the macro list
 * the function goes over the source file and looks for macro definitions
 * when it finds one it adds it to a list
 * if an error occurs returns 1
 * otherwise returns 0 */
int find_macros(FILE** tempfd,mpointer* head,char* filename);

/* head is the pointer to the pointer of the first link in the macro list
 * the function frees the list */
void free_macro_list(mpointer* head);

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

/* head is the pointer to the pointer of the first link in the macro list
 * mname is the macro name to be added
 * mac is the macro content to be added
 * the function adds another node to the list which contains the macro name and the macro itself
 * returns 0 if all went well
 * otherwise returns 1*/
int add_mnode(mpointer* head, char* mname, char* mac);

