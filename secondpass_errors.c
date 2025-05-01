#include "secondpass_errors.h"

/* if there were errors in first pass then this function is called
 * it goes over the files and ignores errors that were found by firstpass
 * it checks is labels that were used as operands and parameters were actually defined */
int error_check(int* IC, int* DC,FILE** source,symnode* symhead,char* filename)
{
    char line[MAX_LINE]; /* hold a line from the file */
    char* word; /* holds a string from the line */
    int counter=0; /* keeps track of the number of the line we are reading */
    int errors=0; /* a flag that turns on (1) when we encounter an error */
    int label_found=0; /* a flag that turns on when a label is found */
    int size; /* keeps the size of word */
    int command;  /* will hold the numeric value that corresponds to the command */
    char op1[MAX_LINE]; /* holds the first operand */
    char op2[MAX_LINE]; /* holds the second operand */
    char temparr[MAX_LINE]; /* sometimes used to save the contents of word */
    char label_name[MAX_LINE]; /* temporarily holds the name of the label */
    int modify=0; /* a flag that signals that a label node needs to be modified */
    char* label_pointer;

    FILE* obfile=NULL; /* will point to the .ob file */
    FILE* entfile=NULL; /* will point to the .ent file */
    FILE* extfile=NULL; /* will point to the .ext file */

    if(open_files(&obfile,&entfile,&extfile,filename,IC,DC)==1) /* creating the files */
    {
        exit(1);
    }


    while(fgets(line,MAX_LINE,*source) != NULL)     /* getting a line from the file */
    {
        counter++;
        word = strtok(line," \n"); /* getting a string from the file that is surrounded with white space */

        if(word==NULL)
        {
            continue;
        }

        size = strlen(word);

        if((label_pointer=strchr(word,':')) != NULL ) /* if the word has a label definition */
        {
            if(word==label_pointer) /* if the label is empty */
            {
                continue;
            }
            strncpy(label_name,word,label_pointer-word);
            label_name[label_pointer-word]='\0';


            if(islabel_no_errors(label_name, &errors) == 1) /* checking if it's a valid label */
            {
                continue;
            }

            if (check_name(label_name) == 1) /* checking if the label is not a reserved word */
            {
                continue;
            }
            if (check_lists_no_errors(label_name, symhead, &modify) == 1) /* checking if the label was already used */
            {
                continue;
            }

            if(word[size-1] == ':') /* if the whole word was a label definition */
            {
                word = strtok(NULL, " \n"); /* getting whatever is after the label */
                if (word == NULL) /* there is nothing after the label */
                {
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
            if(label_found==1)
            {
                label_found=0;
            }

            if(command == RTS || command == STOP) /* if the command doesn't need operands */
            {
                continue;
            } /* end of if command needs no operands */


            if(command == NOT || command == CLR || command == INC || command == DEC ||
               command == JMP || command == BNE || command == RED || command == PRN ||
               command == JSR)                                  /* if the command needs one operand */
            {
                (*IC)++;
                word = strtok(NULL," \n"); /* getting the operand */
                if(word==NULL) /* if the operand is missing */
                {
                    errors=1;
                    continue;
                }

                if(whataction(word) != -1) /* checking that the operand is not a reserved word */
                {
                    continue;
                }

                if(word[size-1] == ')' )      /* checking if it's a jump operand */
                {
                    if(strchr(word,'(') != NULL)
                    {
                        check_jump_no_errors(word,IC,filename,counter,symhead,&extfile);
                    }
                    continue;
                }

                if(islabel_no_errors(word, &errors) == 0) /* the operand is a label */
                {
                    get_address(symhead,word,counter,filename,&extfile,IC); /* checking that the label was referenced */
                }

                continue;

            } /* end of if the command needs 1 operand */


            if(command == MOV || command == LEA || command == CMP || command == ADD || command == SUB)  /* the command needs 2 operands */
            {
                (*IC)++;
                word = strtok(NULL, "\n"); /* getting the every thing after the command */
                if (word == NULL)
                {
                    continue;
                }
                strcpy(temparr, word);
                if (check_commas_no_errors(temparr) == 1) /* checking the number of commas */
                {
                    continue;
                }
                if (get_ops_no_errors(temparr, op1, op2) == 1) /* getting the operands into op1 and op2 */
                {
                    continue;
                }
                if (whataction(op1) != -1 || whataction(op2) != -1) /* one of the operands is a reserved word */
                {
                    continue;
                }
                if(islabel_no_errors(op1, &errors) == 0) /* the operand is a label */
                {
                    get_address(symhead,op1,counter,filename,&extfile,IC); /* checking that the label was referenced */
                }
                if(islabel_no_errors(op2, &errors) == 0) /* the operand is a label */
                {
                    get_address(symhead,op2,counter,filename,&extfile,IC); /* checking that the label was referenced */
                }


            } /* end of the command needs two operands */


        } /* end of if command is an action */


        if(command>=DATA && command<=EXTERN) /* the command is a direction */
        {
            if(label_found==1)
            {
                label_found=0;
            }
            continue;
        } /* end of if command is a direction */

        if(command==-1) /* unknown command */
        {
            continue;
        }

        word = strtok(NULL," \n");  /* checking for extra text after the last operand */
        if(word != NULL)
        {
            continue;
        }

    } /* no more lines from the file */



    check_extern_nodes(symhead,filename);


    delete_files(filename,&obfile,&entfile,&extfile);

    return 0;
}

/* the function checks if a label was not used */
int islabel_no_errors(char *word, int *errors)
{
    int i;
    int size = strlen(word);

    if(size>30)
    {
        *errors=1;
        return 1;
    }
    if(isalpha(word[0])==0) /* if the first char is not a letter */
    {
        *errors=1;
        return 1;
    }
    for(i=1;i<size;i++) /* checking the other chars of the label */
    {
        if(isalnum(word[i])==0)
        {
            *errors=1;
            return 1;
        }
    }
    if(check_name(word)==1)
    {
        return 1;
    }

    return 0;
}


/* the function compares word with the label name from the label list
 * the function does not report errors that were found in first pass */
int check_lists_no_errors(char *word, symnode *symname, int *modify)
{

    symnode temp = *symname;

    while(temp != NULL)
    {
        if(strcmp(word,temp->symname)==0)
        {
            if(temp->isextern==1)
            {
                return 1;
            }
            if(temp->isentry==1 && temp->isinternal==1)
            {
                return 1;
            }
            if(temp->isentry==1 && temp->isinternal==0) /* the label was defined as entry */
            {
                temp->isinternal=1;
                *modify=1;
                return 0;
            }
            if(temp->isinternal==1)
            {
                return 1;
            }

        }
        temp=temp->next;
    }
    return 0;
}


/* this function checks if there is a comma that separates the operands
 * the function does not report errors that were found in firstpass*/
int check_commas_no_errors(char *word)
{
    char* com1 = strchr(word,',');
    char* com2 = strrchr(word,',');

    if(com1 == NULL && com2 == NULL) /* there are no commas */
    {
        return 1;
    }
    if(com1==com2) /* com1 and com2 point to the same comma */
    {
        return 0;
    }
    return 1;
}

/* get_ops copies the operands into op1 and op2
 * the function does not report errors that were found in first pass*/
int get_ops_no_errors(char *word, char *op1, char *op2)
{
    int i;
    int j;
    if(word[0]==',')
    {
        return 1;
    }
    for(i=0;word[i]=='\t' || word[i]==' ';i++) /* skipping white space */
    {

    }
    for(;word[i] != ',' && word[i] != '\t' && word[i] != ' ';i++) /* getting the string until the , or white space*/
    {
        op1[i]=word[i];
    }
    op1[i]='\0';

    if(word[i]==',')
    {
        i++;
    }
    else /* need to look for , */
    {
        i++;
        for(;word[i]=='\t' || word[i]==' ';i++) /* skipping white space */
        {

        }
        if(word[i] != ',') /* there is no , between operands */
        {
            return 1;
        }
        i++;
    }
    for(;word[i]=='\t' || word[i]==' ';i++) /* skipping white space */
    {

    }
    for(j=0;word[i] != '\0' && word[i] != '\t' && word[i] != ' ';i++,j++) /* getting the second operand */
    {
        op2[j]=word[i];
    }
    op2[j]='\0';
    if(op2[0]=='\0') /* if the second operand is missing */
    {
        return 1;
    }
    while(word[i] != '\0') /* checking for extra text */
    {
        if(word[i] != ' ' && word[i] != '\t')
        {
            return 1;
        }
        i++;
    }

    return 0;
}


/* the function checks if a label was not used in a jump parameter*/
int check_jump_no_errors(char* word, int* IC,char* filename,int counter,symnode* symhead,FILE** extfile)
{
    int placeholder=0; /* created so that we can use some functions*/
    char par1[MAX_LINE]; /* holds the first parameter */
    char par2[MAX_LINE]; /* holds the second parameter */
    char label[MAX_LINE]; /* holds the label */
    char* lpointer=label;
    int i=0;
    int j=0;
    int t=0;

    while(word[i] != '(') /* getting the label */
    {
        label[i]=word[i];
        i++;
    }
    label[i]='\0';
    if(label[0]=='\0') /* missing label to be jumped to*/
    {
        return 1;
    }

    if(islabel_no_errors(lpointer, &placeholder) == 0) /* if the word is a label definition */
    {
        if (check_name(label) == 1) /* checking if the label is not a reserved word */
        {
            return 1;
        }
        (*IC)++;
    }

    i++;
    while(word[i] != ',' && word[i] != ')') /* getting the first parameter */
    {
        par1[j]=word[i];
        i++;
        j++;
    }
    par1[j]='\0';

    if(word[i] == ')') /* there was no comma */
    {
        return 1;
    }
    if(par1[0]=='\0') /* missing parameter */
    {
        return 1;
    }
    i++;

    while(word[i] != ')')         /* getting the second parameter */
    {
        par2[t]=word[i];
        i++;
        t++;
    }
    par2[t]='\0';

    if(par2[0]=='\0') /* missing parameter */
    {
        return 1;
    }

    /* checking parameters */

    if(islabel_no_errors(par1, &placeholder) == 0) /* the operand is a label */
    {
        get_address(symhead,par1,counter,filename,extfile,IC); /* checking that the label was referenced */
    }
    if(islabel_no_errors(par2, &placeholder) == 0) /* the operand is a label */
    {
        get_address(symhead,par2,counter,filename,extfile,IC); /* checking that the label was referenced */
    }

    return 0;
}