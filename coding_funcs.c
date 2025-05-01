#include "coding_funcs.h"


/* this function gets the binary number of the command and puts it into memword */
void code_command(int* memword,int command)
{
    int i;
    int j;
    int num=command;

    if(num==0)
    {
        return;
    }

    for(i=0,j=7;num>0;i++,j--)
    {
        memword[j]=num%2;
        num=num/2;
    }
}

/* this function sets all slots to 0 */
void initialize_memword(int* memword)
{
    int i;

    for(i=0;i<BITS_OF_WORD;i++)
    {
        memword[i]=0;
    }
}

/* prints the memword into the .ob file */
void print_word(int* memword,FILE** obfile,int* IC)
{
    int i;

    fprintf(*obfile,"0%d   ",*IC);
    for(i=0;i<BITS_OF_WORD;i++)
    {
        if(memword[i]==0)
        {
            fprintf(*obfile,".");
        }
        else
        {
            fprintf(*obfile,"/");
        }
    }
    fprintf(*obfile,"\n");
}

/* prints the binary representation of a destination register */
void print_one_reg(int num, FILE** obfile,int* IC)
{
    if(num==0)
    {
        fprintf(*obfile,"0%d   ..............\n",*IC);
    }
    if(num==1)
    {
        fprintf(*obfile,"0%d   .........../..\n",*IC);
    }
    if(num==2)
    {
        fprintf(*obfile,"0%d   ........../...\n",*IC);
    }
    if(num==3)
    {
        fprintf(*obfile,"0%d   ..........//..\n",*IC);
    }
    if(num==4)
    {
        fprintf(*obfile,"0%d   ........./....\n",*IC);
    }
    if(num==5)
    {
        fprintf(*obfile,"0%d   ........././..\n",*IC);
    }
    if(num==6)
    {
        fprintf(*obfile,"0%d   .........//...\n",*IC);
    }
    if(num==7)
    {
        fprintf(*obfile,"0%d   .........///..\n",*IC);
    }
}

/* this function prints the binary representation of a decimal number. */
void print_num(char* num,int* IC, FILE** obfile,int* memword)
{
    int i;
    int j;
    int counter;
    int number=atoi(num);
    int binnum[BITS_OF_WORD]; /* will hold the reversed binary number */
    initialize_memword(memword);
    initialize_memword(binnum);
    if(number==0)
    {
        fprintf(*obfile,"0%d   ..............\n",*IC);
        return;
    }

    if(number>0)
    {
        for (i = 0; number > 0; i++) /* getting the binary number in a reversed order */
        {
            binnum[i] = number % 2;
            number = number / 2;
        }
        counter=i;
        for(j=11,i=0;i<=counter;i++,j--)
        {
            memword[j]=binnum[i];   /* placing the binary number in memword to be printer */
        }
    }

    if(number<0)
    {
        number=-number;
        for (i = 0; number > 0; i++) /* getting the binary number in a reversed order */
        {
            binnum[i] = number % 2;
            number = number / 2;
        }
        counter=i;
        for(j=11,i=0;i<=counter;i++,j--) /* placing the binary number in memword to be printer */
        {
            memword[j]=binnum[i];
        }
        for(i=0;i<12;i++) /* flipping all the bits */
        {
            if(memword[i]==1)
            {
                memword[i]=0;
            }
            else
            {
                memword[i]=1;
            }
        }
        for(i=11;memword[i] == 1;i--)  /* "adding" 1 to the binary number */
        {
            memword[i]=0;
        }
        memword[i]=1;


    }
    print_word(memword,obfile,IC);
}

/* this function prints the address of a label */
void print_address(int num,int* IC,FILE** obfile,int* memword)
{
    int i;
    int j;
    int number=num;
    int counter;
    int binnum[BITS_OF_WORD]; /* will hold the reversed binary number */
    initialize_memword(binnum);



    for (i = 0; number > 0; i++) /* getting the binary number in a reversed order */
    {
        binnum[i] = number % 2;
        number = number / 2;
    }
    counter=i;
    for(j=11,i=0;i<counter;i++,j--)
    {
        memword[j]=binnum[i];   /* placing the binary number in memword to be printed */
    }
    print_word(memword,obfile,IC);
}

/* codes the parameters into memword and then memword is printed to the .ob file*/
void code_par_type(int* memword,int* IC, FILE** obfile,int par1type,int par2type)
{
    if(par1type==DIRECT)
    {
        memword[1]=1;
    }
    if(par1type==REGISTER)
    {
        memword[0]=1;
        memword[1]=1;
    }
    if(par2type==DIRECT)
    {
        memword[3]=1;
    }
    if(par2type==REGISTER)
    {
        memword[2]=1;
        memword[3]=1;
    }

    print_word(memword,obfile,IC);
}

/* this function is used when 2 registers are the operands.
 * it combines them into memword wo be printed to the .ob file. */
void code_two_regs(char* par1, char* par2,int* IC,FILE** obfile)
{
    int i;
    int j;
    int reg1num; /* reg1num holds the number of the first register */
    int reg2num; /* reg2num holds the number of the second register */
    int reg1bin[BITS_OF_WORD]; /* holds the binary representation of the first register */
    int reg2bin[BITS_OF_WORD]; /* holds the binary representation of the second register */
    int memword[BITS_OF_WORD]; /* this is the memory word that will be printed */

    char* saved_name[8];
    saved_name[0]="r0";
    saved_name[1]="r1";
    saved_name[2]="r2";
    saved_name[3]="r3";
    saved_name[4]="r4";
    saved_name[5]="r5";
    saved_name[6]="r6";
    saved_name[7]="r7";

    initialize_memword(reg1bin); /* setting all the bits to 0 */
    initialize_memword(reg2bin);
    initialize_memword(memword);

    for(i=0;i<8;i++) /* getting the registers number */
    {
        if(strcmp(par1,saved_name[i])==0)
        {
            reg1num=i;
        }
        if(strcmp(par2,saved_name[i])==0)
        {
            reg2num=i;
        }
    }

    get_bin(reg1bin,reg1num); /* getting the binary numbers of the registers */
    get_bin(reg2bin,reg2num);

    for(i=11,j=BITS_OF_WORD-1;i>5;i--,j--) /* putting the binary numbers into memword */
    {
        memword[i]=reg2bin[j];
    }
    for(j=BITS_OF_WORD-1;i>=0;i--,j--)
    {
        memword[i]=reg1bin[j];
    }

    print_word(memword,obfile,IC); /* printing memword */

}

/* this function prints the binary representation of a destination register */
void print_1_reg(char num, FILE** obfile,int* IC)
{
    if(num=='0')
    {
        fprintf(*obfile,"0%d   ..............\n",*IC);
    }
    if(num=='1')
    {
        fprintf(*obfile,"0%d   .........../..\n",*IC);
    }
    if(num=='2')
    {
        fprintf(*obfile,"0%d   ........../...\n",*IC);
    }
    if(num=='3')
    {
        fprintf(*obfile,"0%d   ..........//..\n",*IC);
    }
    if(num=='4')
    {
        fprintf(*obfile,"0%d   ........./....\n",*IC);
    }
    if(num=='5')
    {
        fprintf(*obfile,"0%d   ........././..\n",*IC);
    }
    if(num=='6')
    {
        fprintf(*obfile,"0%d   .........//...\n",*IC);
    }
    if(num=='7')
    {
        fprintf(*obfile,"0%d   .........///..\n",*IC);
    }
}

/* prints the source register.
 * using get_bin we get the binary number of the register.
 * then, that number is placed in memword in the correct location.
 * then, memword is printed to the .ob file */
void print_left_reg(char regnum,FILE** obfile,int* IC)
{
    int i;
    int j;
    int reg1bin[BITS_OF_WORD]; /* will hold the binary number */
    int memword[BITS_OF_WORD]; /* the memory word that will be printed */


    initialize_memword(reg1bin); /* setting every thing to 0 */
    initialize_memword(memword);



    get_bin(reg1bin,regnum-'0'); /* getting the binary number into reg1bin */


    for(i=5, j=BITS_OF_WORD-1;i>=0;i--,j--) /* putting the binary number in the correct place in memword */
    {
        memword[i]=reg1bin[j];
    }

    print_word(memword,obfile,IC);
    (*IC)++;

}

/* this function codes the operand types into memword */
void code_2_op_type(int* memword,int op1,int op2)
{
    if(op1==DIRECT) /* if op1 is a number */
    {
        memword[9]=1;
    }
    if(op1==REGISTER) /* if op1 is a register */
    {
        memword[8]=1;
        memword[9]=1;
    }
    if(op2==DIRECT) /* if op2 is a number */
    {
        memword[11]=1;
    }
    if(op2==REGISTER) /* if op2 is a register */
    {
        memword[10]=1;
        memword[11]=1;
    }
}

/* this function takes the string from the .am file.
 * for each char it encounters, it will crate a memory node and store its value inside*/
int code_string(int* DC,mempointer* head)
{
    char* word; /* will hold the string to be checked */
    int i;
    int j=0;
    char arr[MAX_LINE]; /* will hold the string in "" */
    mempointer cur; /* will point to the newly added node */

    word = strtok(NULL,"\n"); /* getting every thing after the .string direction */

    for(i=0; word[i] != '\"'; i++) /* checking for text before the string */
    {

    }

    for(i=i+1; word[i] != '\"' ; i++,j++) /* getting the string in " " */
    {
        arr[j]=word[i];
    }
    arr[j]='\0';

    for(i=0;i<= strlen(arr);i++)
    {
        if(add_word_node(head,DC,&cur)==1)
        {
            printf("\nError, memory allocation failed\n");
            exit(1);
        }
        cur->dc=*DC;
        get_bin(cur->memword,arr[i]);
        (*DC)++;
    }

    return 0;
}

/* this function goes over the data line.
 * each number it finds gets inserted into the memory list to be printed later*/
int code_data(int *DC,mempointer* memlist)
{
    char* word = strtok(NULL,"\n"); /* getting the rest of the line */
    int i=0;
    int j=0;
    char arr[MAX_LINE]; /* arr will hold a potential number */
    mempointer cur; /* cur will point to the newly added node */


    while(word[i] != '\0')
    {
        if(word[i] == '\t' || word[i] == ' ') /* skip white space */
        {
            i++;
            continue;
        }

        if(word[i] != ',') /*found the start of a string */
        {

            while(word[i] != '\0' && word[i] != ',' && word[i] != ' ' && word[i] != '\t') /* getting a string without white space and a comma */
            {
                arr[j]=word[i];
                i++;
                j++;
            }
            arr[j]='\0';
            j=0;
            if(add_word_node(memlist,DC,&cur)==1) /* adding a node to the word list */
            {
                printf("\nError, memory allocation failed\n");
                exit(1);
            }

            get_bin(cur->memword,atoi(arr));  /* getting the binary number */

            (*DC)++;

        } /* end of finding a string */

        if(word[i] == ',')
        {
            i++;
        }

    } /* end of analyzing the line */

    return 0;
}

