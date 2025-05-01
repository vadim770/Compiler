#include "secondpassfuncs.h"

/* this function oversees the second pass.
 * the function creates the .ob .ext .ent files.
 * then .ent is being filled.
 * then, the function gets a line from the .am file and breaks it into single words with strtok()
 * memword is used to store the binary codes of actions and operands.
 * each word is evaluated and coded into memword. and then printed
 * the function checks that all the labels that were defined are indeed used, if not then it is considered an error.
 * if any errors were found then the .ob .ext .ent files will be deleted */
int secondpass(int* IC, int* DC, FILE** source,symnode* symhead,char* filename, int prv_errors)
{
    FILE* obfile=NULL; /* will point to the .ob file */
    FILE* entfile=NULL; /* will point to the .ent file */
    FILE* extfile=NULL; /* will point to the .ext file */
    int memword[BITS_OF_WORD]; /* will be used to code a word from the file */
    int errors=prv_errors; /* a flag the indicates that there was an error */
    char line[MAX_LINE]; /* hold a line from the file */
    char* word; /* holds a string from the line */
    int counter=0; /* keeps track of the number of the line we are reading */
    int label_found=0; /* a flag that turns on when a label is found */
    int size; /* keeps the size of word */
    int optype; /* will hold the numeric value that corresponds to the operand */
    int command;  /* will hold the numeric value that corresponds to the command */
    char op1[MAX_LINE]; /* holds the first operand */
    char op2[MAX_LINE]; /* holds the second operand */
    int op1type; /* holds the type of the first operand */
    int op2type; /* holds the type of the second operand */
    char temparr[MAX_LINE]; /* sometimes used to save the contents of word */
    char label_name[MAX_LINE]; /* temporarily holds the name of the label */
    char* label_pointer; /* will point to a label */
    mempointer memhead=NULL;

    if(open_files(&obfile,&entfile,&extfile,filename,IC,DC)==1) /* creating the files */
    {
        exit(1);
    }

    (*IC)=100;
    (*DC)=0;
    fill_entry(symhead,&entfile); /* prints the entry labels in the ent file */


    while(fgets(line,MAX_LINE,*source) != NULL)     /* getting a line from the file */
    {
        initialize_memword(memword); /* putting 0 in all slots */
        counter++;
        word = strtok(line," \n"); /* getting a string from the file that is surrounded with white space */
        if(word==NULL)
        {
            continue;
        }
        size = strlen(word);

        if((label_pointer=strchr(word,':')) != NULL ) /* if the word has a label definition */
        {

            strncpy(label_name,word,label_pointer-word);
            label_name[label_pointer-word]='\0';

            if(word[size-1] == ':') /* if the whole word was a label definition */
            {
                word = strtok(NULL, " \n"); /* getting whatever is after the label */
                if (word == NULL) /* there is nothing after the label */
                {
                    errors = 1;
                    continue;
                }
            }
            else
            {
                word=label_pointer+1; /* word now points to whatever is after the : */
            }
            label_found=1;
        } /* end of label was found */

        command = whataction(word); /* command now holds the command */


        if(command>=MOV && command<=STOP ) /* the command is an action */
        {
            code_command(memword,command); /* coding the command to memword */

            if(label_found==1)
            {
                label_found=0;
            }

            if(command == RTS || command == STOP) /* if the command doesn't need operands */
            {
                if(command == RTS)
                {
                    fprintf(obfile,"0%d   ....///.......\n",*IC);
                }
                if(command == STOP)
                {
                    fprintf(obfile,"0%d   ....////......\n",*IC);
                }
                (*IC)++;

            } /* end of if command needs no operands */


            if(command == NOT || command == CLR || command == INC || command == DEC ||
               command == JMP || command == BNE || command == RED || command == PRN ||
               command == JSR)                                  /* if the command needs one operand */
            {

                word = strtok(NULL," \n"); /* getting the operand */


                optype = code_operand(word, symhead, filename, counter, IC, memword, &obfile, &extfile);   /* coding the operand to memword */
                if(optype==-1)
                {
                    errors=1;
                    continue;
                }
                if(optype==JUMP) /* if the operand is a jump operand with parameters */
                {
                   if( code_jump(word,symhead,filename,counter,IC,memword,&obfile,&extfile)==1) /* coding the action, the label and the parameters */
                   {
                       errors=1;
                       continue;
                   }
                }

            } /* end of if the command needs 1 operand */


            if(command == MOV || command == LEA || command == CMP || command == ADD || command == SUB)  /* the command needs 2 operands */
            {

                word = strtok(NULL, "\n"); /* getting every thing after the command */

                strcpy(temparr, word);

                if (get_ops(temparr, filename, counter, op1, op2) == 1) /* getting the operands into op1 and op2 */
                {
                    errors = 1;
                    continue;
                }

                op1type = check_operand(op1,  filename, counter, IC); /* getting the operand type */
                if(op1type==-1)
                {
                    errors=1;
                    continue;
                }
                op2type = check_operand(op2,  filename, counter, IC);  /* getting the operand type */
                if(op2type==-1)
                {
                    errors=1;
                    continue;
                }
                (*IC)=(*IC)-2;   /* undoing the increase of IC that was being done in check_operand */
                code_2_op_type(memword,op1type,op2type);  /* coding the operand types to memword */
                print_word(memword,&obfile,IC);
                initialize_memword(memword);
                (*IC)++;

                if (op1type == REGISTER && op2type == REGISTER) /* if there are 2 registers, they are sharing the same word */
                {
                    code_two_regs(op1,op2,IC,&obfile); /* coding 2 registers */
                    (*IC)++;
                    continue;
                }

                if(op1type != REGISTER)
                {
                    if(code_operand2(op1,symhead,filename,counter,IC,memword,&obfile,&extfile)==-1)
                    {
                        errors=1;
                        continue;
                    }
                }
               if(op1type==REGISTER) /* if the source operand is a register it needs to be printed to the left */
               {
                   print_left_reg(op1[1],&obfile,IC);
               }

                if(code_operand2(op2,symhead,filename,counter,IC,memword,&obfile,&extfile)==-1)
                {
                    errors=1;
                    continue;
                }


            } /* end of the command needs two operands */


        } /* end of if command is an action */


        if(command>=DATA && command<=EXTERN) /* the command is a direction */
        {
            if(command == EXTERN || command==ENTRY) /* there is nothing to be done with extern or entry */
            {
                continue;
            }

            if(command == DATA || command == STRING)
            {
                if(label_found==1)
                {
                    label_found=0;
                }

                if(command==DATA)
                {
                    if(code_data(DC,&memhead)==1)
                    { /* fatal error encountered */
                        delete_files(filename,&obfile,&entfile,&extfile);
                        fclose(obfile);
                        return 1;

                    }
                }
                if(command==STRING)
                {
                    if(code_string(DC,&memhead)==1)
                    {
                        delete_files(filename,&obfile,&entfile,&extfile);
                        fclose(obfile);
                        return 1;
                    }
                }

            }

        } /* end of if command is a direction */


    } /* no more lines from the file */


    if(check_extern_nodes(symhead,filename)==1) /* checking that all the extern nodes were used */
    {
        errors=1;
    }


    if(errors==1 || prv_errors==1) /* if any errors were found */
    {
        delete_files(filename,&obfile,&entfile,&extfile);
        free_word_list(&memhead);
        return 1;
    }

    print_data(&memhead,IC,&obfile);
    delete_empty(filename,&entfile,&extfile); /* deleting empty files */
    free_word_list(&memhead);
    fclose(obfile);


    return 0;
}




/* this function is used when getting an action the needs 2 operands.
 * the function will check what is the operand type and after that it will code and print it to the .ob file.
 * if an external label was found it will print it to the .ext file.*/
int code_operand2(char* word,symnode* symhead ,char* filename, int counter,int* IC,int* memword,FILE** obfile,FILE** extfile)
{
    int status; /* will be used to get output from functions */
    int i;
    int j;
    int x;
    char arr[MAX_LINE]; /* arr holds the number if the operand is a number */
    int size = strlen(word);
    char* saved_name[8];
    saved_name[0]="r0";
    saved_name[1]="r1";
    saved_name[2]="r2";
    saved_name[3]="r3";
    saved_name[4]="r4";
    saved_name[5]="r5";
    saved_name[6]="r6";
    saved_name[7]="r7";

    for(i=0;i<8;i++) /* checking if the operand is a register */
    {
        if(strcmp(word,saved_name[i])==0)
        {
            print_one_reg(i,obfile,IC); /* codes the destination register */
            (*IC)++;
            return REGISTER;
        }
    }


    if(word[0]=='#') /* there might be a number */
    {

        if( isdigit(word[1]) || word[1]=='+' || word[1]=='-') /* after # there might be a number */
        {
            if(isdigit(word[1]) || word[1]=='-' || word[1]=='+')
            {
                arr[0]=word[1];
            }
            for(x=1,j=2;j<size;j++,x++)
            {
                arr[x]=word[j];
            }
            arr[x]='\0';
        }
        print_num(arr,IC,obfile,memword);
        (*IC)++;
        return IMMEDIATE;
    }

    /* checking if the operand is a label */


        status = get_address(symhead,word,counter,filename,extfile,IC);
        initialize_memword(memword);
        if(status==-2) /* got error */
        {
            return -1;
        }
        if(status==-1) /* the label is external */
        {
            memword[13]=1;
            print_word(memword,obfile,IC);
        }
        else /* the label is internal */
        {
            memword[12]=1;
            print_address(status,IC,obfile,memword);
        }
        (*IC)++;
        return DIRECT;


}


/* this function codes and prints the jump operand with parameters.
 * first it gets the label to be jumped to, the first operand and the second operand.
 * it then checks the operand types and codes it into memword.
 * it then codes the operands and prints them to the .ob file.
 * if the operands are external labels then they are also printed to the .ext file */
int code_jump(char* word,symnode* head, char* filename,int counter,int* IC, int* memword,FILE** obfile,FILE** extfile)
{
    char par1[MAX_LINE]; /* holds the first parameter */
    char par2[MAX_LINE]; /* holds the second parameter */
    char label[MAX_LINE]; /* holds the label */
    int par1type; /* holds the type of the first parameter */
    int par2type; /* holds the type of the second parameter */
    int i=0;
    int j=0;
    int t=0;
    int regnum=0; /* will keep track of how many register there are */
    int status; /* used to get output from functions */

    memword[10]=1;

    while(word[i] != '(') /* getting the label */
    {
        label[i]=word[i];
        i++;
    }
    label[i]='\0';
    i++;
    while(word[i] != ',' && word[i] != ')') /* getting the first parameter */
    {
        par1[j]=word[i];
        i++;
        j++;
    }
    par1[j]='\0';
    i++;
    while(word[i] != ')')         /* getting the second parameter */
    {
        par2[t]=word[i];
        i++;
        t++;
    }
    par2[t]='\0';

    /* checking parameters */

    par1type = check_parameter(par1,filename,counter,&regnum);

    par2type = check_parameter(par2,filename,counter,&regnum);

    code_par_type(memword,IC,obfile,par1type,par2type); /* coding the parameters into memword */
    (*IC)++;
    initialize_memword(memword);

    code_operand2(label,head,filename,counter,IC,memword,obfile,extfile); /* check and print the label address that is being jumped to */

    if(regnum==2) /* if there are 2 registers in the jump operand */
    {
        code_two_regs(par1,par2,IC,obfile);
        (*IC)++;
        return 0;
    }

    initialize_memword(memword); /* resetting memword for the first parameter */

    if(par1type==IMMEDIATE)
    {
        print_num(par1+1,IC,obfile,memword); /* +1 because par1[0] is # */
        (*IC)++;
    }
    if(par1type==DIRECT)
    {
        status = get_address(head,par1,counter,filename,extfile,IC);
        if(status==-2) /* got error */
        {
            return 1;
        }
        if(status==-1) /* the label is external */
        {
            memword[13]=1;
            print_word(memword,obfile,IC);
        }
        else
        {
            memword[12]=1;
            print_address(status,IC,obfile,memword);
        }
        (*IC)++;
    }
    if(par1type==REGISTER)
    {
        print_left_reg(par1[1],obfile,IC);
    }

    initialize_memword(memword); /* resetting memword for the second parameter */

    if(par2type==IMMEDIATE)
    {
        print_num(par2+1,IC,obfile,memword); /* +1 because par2[0] is # */
        (*IC)++;
    }
    if(par2type==DIRECT)
    {
        status = get_address(head,par2,counter,filename,extfile,IC);
        if(status==-2) /* got error */
        {
            return 1;
        }
        if(status==-1) /* the label is external */
        {
            memword[13]=1;
            print_word(memword,obfile,IC);
        }
        else
        {
            memword[12]=1;
            print_address(status,IC,obfile,memword);
        }
        (*IC)++;

    }
    if(par2type==REGISTER)
    {
        print_1_reg(par2[1],obfile,IC);
        (*IC)++;
    }

    return 0;
}



/* this function gets a number and puts the binary number into the array "binary" */
void get_bin(int* binary,int num)
{
    int temp[BITS_OF_WORD];
    int i;
    int j;

    initialize_memword(temp);

    if(num>0)
    {
        for (i = 0; num > 0; i++) /* getting the binary number in a reversed order */
        {
            temp[i] = num % 2;
            num = num / 2;
        }
        for (j = BITS_OF_WORD - 1, i = 0; i < BITS_OF_WORD; j--, i++) /* reversing the number and placing it into binary */
        {
            binary[j] = temp[i];
        }
    }

    if(num<0)
    {
        num=-num;
        for (i = 0; num > 0; i++) /* getting the binary number in a reversed order */
        {
            temp[i] = num % 2;
            num = num / 2;
        }
        for (j = BITS_OF_WORD - 1, i = 0; i < BITS_OF_WORD; j--, i++) /* reversing the number and placing it into binary */
        {
            binary[j] = temp[i];
        }
        for(i=0;i<BITS_OF_WORD;i++) /* flipping all the bits */
        {
            if(binary[i]==1)
            {
                binary[i]=0;
            }
            else
            {
                binary[i]=1;
            }
        }
        for(i=BITS_OF_WORD-1;binary[i] == 1;i--)  /* "adding" 1 to the binary number */
        {
            binary[i]=0;
        }
        binary[i]=1;


    }

}




/* this function is used when there is only one operand to code and print.
 * the function checks what is the operand and codes it accordingly. then, it is printed.
 * the function does not code jump operands with parameters.*/
int code_operand(char *word, symnode *symhead, char *filename, int counter, int *IC, int *memword, FILE **obfile,
                 FILE **extfile)
{
    int status; /* used to get output from functions */
    int i;
    int j;
    int x;
    char arr[MAX_LINE]; /* arr holds the number if the operand is a number */
    int size = strlen(word);

    char* saved_name[8];
    saved_name[0]="r0";
    saved_name[1]="r1";
    saved_name[2]="r2";
    saved_name[3]="r3";
    saved_name[4]="r4";
    saved_name[5]="r5";
    saved_name[6]="r6";
    saved_name[7]="r7";

    for(i=0;i<8;i++) /* checking if the operand is a register */
    {
        if(strcmp(word,saved_name[i])==0)
        {
            memword[10]=1;
            memword[11]=1;
            print_word(memword,obfile,IC); /* print the command with the operand type */
            (*IC)++;
            print_one_reg(i,obfile,IC); /* print the destination register */
            (*IC)++;
            return REGISTER;
        }
    }

    if(word[size-1] == ')' )      /* checking if it's a jump operand */
    {
        if(strchr(word,'(') != NULL)
        {
                return JUMP; /* the coding of a jump operand is being done by code_jump */
        }
    }

    if(word[0]=='#') /* there might be a number */
    {

        if( isdigit(word[1]) || word[1]=='+' || word[1]=='-') /* after # there might be a number */
        {
            if(isdigit(word[1]) || word[1]=='-' || word[1]=='+')
            {
                arr[0]=word[1];
            }
            for(x=1,j=2;j<size;j++,x++)
            {
                arr[x]=word[j];
            }
            arr[x]='\0';
        }
        print_word(memword,obfile,IC); /* print the command with the operand type */
        (*IC)++;
        print_num(arr,IC,obfile,memword); /* print the number */
        (*IC)++;
        return IMMEDIATE;
    }

    /* checking if the operand is a label */


        memword[11]=1;
        print_word(memword,obfile,IC); /* print the command with the operand type */
        (*IC)++;
        status = get_address(symhead,word,counter,filename,extfile,IC);
        initialize_memword(memword);
        if(status==-2) /* got error */
        {
            return -1;
        }
        if(status==-1) /* the label is external */
        {
            memword[13]=1;
            print_word(memword,obfile,IC);
        }
        else
        {
            memword[12]=1;
            print_address(status,IC,obfile,memword);
        }
        (*IC)++;
        return DIRECT;


}

