#include "tables_and_arrays.h"

/* this function initializes the table that contains all the legal addressing types for all the commands */
void initialize_action_table(aop** tpointer)
{
    (*tpointer)[MOV].source[0]=1;
    (*tpointer)[MOV].source[1]=1;
    (*tpointer)[MOV].source[2]=0;
    (*tpointer)[MOV].source[3]=1;

    (*tpointer)[MOV].dest[0]=0;
    (*tpointer)[MOV].dest[1]=1;
    (*tpointer)[MOV].dest[2]=0;
    (*tpointer)[MOV].dest[3]=1;


    (*tpointer)[CMP].source[0]=1;
    (*tpointer)[CMP].source[1]=1;
    (*tpointer)[CMP].source[2]=0;
    (*tpointer)[CMP].source[3]=1;

    (*tpointer)[CMP].dest[0]=1;
    (*tpointer)[CMP].dest[1]=1;
    (*tpointer)[CMP].dest[2]=0;
    (*tpointer)[CMP].dest[3]=1;

    (*tpointer)[ADD].source[0]=1;
    (*tpointer)[ADD].source[1]=1;
    (*tpointer)[ADD].source[2]=0;
    (*tpointer)[ADD].source[3]=1;

    (*tpointer)[ADD].dest[0]=0;
    (*tpointer)[ADD].dest[1]=1;
    (*tpointer)[ADD].dest[2]=0;
    (*tpointer)[ADD].dest[3]=1;

    (*tpointer)[SUB].source[0]=1;
    (*tpointer)[SUB].source[1]=1;
    (*tpointer)[SUB].source[2]=0;
    (*tpointer)[SUB].source[3]=1;

    (*tpointer)[SUB].dest[0]=0;
    (*tpointer)[SUB].dest[1]=1;
    (*tpointer)[SUB].dest[2]=0;
    (*tpointer)[SUB].dest[3]=1;

    (*tpointer)[NOT].source[0]=2;
    (*tpointer)[NOT].source[1]=2;
    (*tpointer)[NOT].source[2]=2;
    (*tpointer)[NOT].source[3]=2;

    (*tpointer)[NOT].dest[0]=0;
    (*tpointer)[NOT].dest[1]=1;
    (*tpointer)[NOT].dest[2]=0;
    (*tpointer)[NOT].dest[3]=1;

    (*tpointer)[CLR].source[0]=2;
    (*tpointer)[CLR].source[1]=2;
    (*tpointer)[CLR].source[2]=2;
    (*tpointer)[CLR].source[3]=2;

    (*tpointer)[CLR].dest[0]=0;
    (*tpointer)[CLR].dest[1]=1;
    (*tpointer)[CLR].dest[2]=0;
    (*tpointer)[CLR].dest[3]=1;

    (*tpointer)[LEA].source[0]=0;
    (*tpointer)[LEA].source[1]=1;
    (*tpointer)[LEA].source[2]=0;
    (*tpointer)[LEA].source[3]=0;

    (*tpointer)[LEA].dest[0]=0;
    (*tpointer)[LEA].dest[1]=1;
    (*tpointer)[LEA].dest[2]=0;
    (*tpointer)[LEA].dest[3]=1;

    (*tpointer)[INC].source[0]=2;
    (*tpointer)[INC].source[1]=2;
    (*tpointer)[INC].source[2]=2;
    (*tpointer)[INC].source[3]=2;

    (*tpointer)[INC].dest[0]=0;
    (*tpointer)[INC].dest[1]=1;
    (*tpointer)[INC].dest[2]=0;
    (*tpointer)[INC].dest[3]=1;

    (*tpointer)[DEC].source[0]=2;
    (*tpointer)[DEC].source[1]=2;
    (*tpointer)[DEC].source[2]=2;
    (*tpointer)[DEC].source[3]=2;

    (*tpointer)[DEC].dest[0]=0;
    (*tpointer)[DEC].dest[1]=1;
    (*tpointer)[DEC].dest[2]=0;
    (*tpointer)[DEC].dest[3]=1;

    (*tpointer)[JMP].source[0]=2;
    (*tpointer)[JMP].source[1]=2;
    (*tpointer)[JMP].source[2]=2;
    (*tpointer)[JMP].source[3]=2;

    (*tpointer)[JMP].dest[0]=0;
    (*tpointer)[JMP].dest[1]=1;
    (*tpointer)[JMP].dest[2]=1;
    (*tpointer)[JMP].dest[3]=0;

    (*tpointer)[BNE].source[0]=2;
    (*tpointer)[BNE].source[1]=2;
    (*tpointer)[BNE].source[2]=2;
    (*tpointer)[BNE].source[3]=2;

    (*tpointer)[BNE].dest[0]=0;
    (*tpointer)[BNE].dest[1]=1;
    (*tpointer)[BNE].dest[2]=1;
    (*tpointer)[BNE].dest[3]=0;

    (*tpointer)[RED].source[0]=2;
    (*tpointer)[RED].source[1]=2;
    (*tpointer)[RED].source[2]=2;
    (*tpointer)[RED].source[3]=2;

    (*tpointer)[RED].dest[0]=0;
    (*tpointer)[RED].dest[1]=1;
    (*tpointer)[RED].dest[2]=0;
    (*tpointer)[RED].dest[3]=1;

    (*tpointer)[PRN].source[0]=2;
    (*tpointer)[PRN].source[1]=2;
    (*tpointer)[PRN].source[2]=2;
    (*tpointer)[PRN].source[3]=2;

    (*tpointer)[PRN].dest[0]=1;
    (*tpointer)[PRN].dest[1]=1;
    (*tpointer)[PRN].dest[2]=0;
    (*tpointer)[PRN].dest[3]=1;

    (*tpointer)[JSR].source[0]=2;
    (*tpointer)[JSR].source[1]=2;
    (*tpointer)[JSR].source[2]=2;
    (*tpointer)[JSR].source[3]=2;

    (*tpointer)[JSR].dest[0]=0;
    (*tpointer)[JSR].dest[1]=1;
    (*tpointer)[JSR].dest[2]=1;
    (*tpointer)[JSR].dest[3]=0;

    (*tpointer)[RTS].source[0]=2;
    (*tpointer)[RTS].source[1]=2;
    (*tpointer)[RTS].source[2]=2;
    (*tpointer)[RTS].source[3]=2;

    (*tpointer)[RTS].dest[0]=2;
    (*tpointer)[RTS].dest[1]=2;
    (*tpointer)[RTS].dest[2]=2;
    (*tpointer)[RTS].dest[3]=2;

    (*tpointer)[STOP].source[0]=2;
    (*tpointer)[STOP].source[1]=2;
    (*tpointer)[STOP].source[2]=2;
    (*tpointer)[STOP].source[3]=2;

    (*tpointer)[STOP].dest[0]=2;
    (*tpointer)[STOP].dest[1]=2;
    (*tpointer)[STOP].dest[2]=2;
    (*tpointer)[STOP].dest[3]=2;
}

/* word might be a valid label
 * the function checks if word is not a reserved word */
int check_name(char* word)
{
    int i;
    char* saved_name[ILLEGAL_NAMES]; /* creating an array with all the reserved words */
    saved_name[0]="r0";
    saved_name[1]="r1";
    saved_name[2]="r2";
    saved_name[3]="r3";
    saved_name[4]="r4";
    saved_name[5]="r5";
    saved_name[6]="r6";
    saved_name[7]="r7";
    saved_name[8]="mov";
    saved_name[9]="cmp";
    saved_name[10]="add";
    saved_name[11]="sub";
    saved_name[12]="not";
    saved_name[13]="clr";
    saved_name[14]="lea";
    saved_name[15]="inc";
    saved_name[16]="dec";
    saved_name[17]="jmp";
    saved_name[18]="bne";
    saved_name[19]="red";
    saved_name[20]="prn";
    saved_name[21]="jsr";
    saved_name[22]="rts";
    saved_name[23]="stop";
    saved_name[24]=".data";
    saved_name[25]=".string";
    saved_name[26]=".entry";
    saved_name[27]=".extern";
    saved_name[28]="mcr";
    saved_name[29]="endmcr";
    saved_name[30]="PSW";

    for(i=0;i<ILLEGAL_NAMES;i++)
    {
        if(strcmp(word,saved_name[i])==0)
        {
            return 1;
        }
    }
    return 0;
}

/* the function compares word to the actions that are available
 * if a match was found we return 0
 * otherwise we return 1 */
int whataction(char* word)
{
    int i;
    char* saved_name[20];
    saved_name[0]="mov";
    saved_name[1]="cmp";
    saved_name[2]="add";
    saved_name[3]="sub";
    saved_name[4]="not";
    saved_name[5]="clr";
    saved_name[6]="lea";
    saved_name[7]="inc";
    saved_name[8]="dec";
    saved_name[9]="jmp";
    saved_name[10]="bne";
    saved_name[11]="red";
    saved_name[12]="prn";
    saved_name[13]="jsr";
    saved_name[14]="rts";
    saved_name[15]="stop";
    saved_name[16]=".data";
    saved_name[17]=".string";
    saved_name[18]=".entry";
    saved_name[19]=".extern";

    for(i=0;i<20;i++)
    {
        if(strcmp(word,saved_name[i])==0)
        {
            return i;
        }
    }
    return -1;
}

