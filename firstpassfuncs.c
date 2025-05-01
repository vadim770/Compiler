#include "firstpassfuncs.h"

/* firstpass oversees the first pass of the assembler.
 * it will make and update the symbol list, and it will search for errors.
 * the function gets a line from the file with fgets.
 * strtok is being used to separate the line to strings that are surrounded with white space, word holds these strings.
 * word will be analyzed by other function to determine what it holds.
 * the function starts by checking if word is a label. if it is then the label name will be saved and label_found will be 1.
 * next the function checks for a command.
 * firstpass will check if it is an action or a direction and how many operands it needs.
 * firstpass will check the operands and will update IC or DC.
 * if any errors were found the error flag turns on */
int firstpass(int* IC, int* DC,FILE** source,symnode* symhead, aop** tpointer,char* filename)
{
    char line[MAX_LINE]; /* hold a line from the file */
    char* word; /* holds a string from the line */
    int counter=0; /* keeps track of the number of the line we are reading */
    int errors=0; /* a flag that turns on (1) when we encounter an error */
    int label_found=0; /* a flag that turns on when a label is found */
    int size; /* keeps the size of word */
    symnode curnode=NULL; /* points to the newest node in the list */
    int optype; /* will hold the numeric value that corresponds to the operand */
    int command;  /* will hold the numeric value that corresponds to the command */
    char op1[MAX_LINE]; /* holds the first operand */
    char op2[MAX_LINE]; /* holds the second operand */
    int op1type; /* holds the type of the first operand */
    int op2type; /* holds the type of the second operand */
    char temparr[MAX_LINE]; /* sometimes used to save the contents of word */
    char label_name[MAX_LINE]; /* temporarily holds the name of the label */
    int modify=0; /* a flag that signals that a label node needs to be modified */
    int status; /* used to get output from certain functions */
    char* label_pointer;

    initialize_action_table(tpointer);

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
                printf("\nError in file %s in line %u, invalid label definition\n",filename,counter);
                continue;
            }
            strncpy(label_name,word,label_pointer-word);
            label_name[label_pointer-word]='\0';


            if(islabel(label_name,counter,filename,&errors)==1) /* checking if it's a valid label */
            {
                continue;
            }

            if (check_name(label_name) == 1) /* checking if the label is not a reserved word */
            {
                errors = 1;
                printf("\nError in file %s in line %u, the label is a reserved word\n", filename, counter);
                continue;
            }
            if (check_lists(label_name, symhead,filename,counter,&modify) == 1) /* checking if the label was already used */
            {
                errors = 1;
                continue;
            }

            if(word[size-1] == ':') /* if the whole word was a label definition */
            {
                word = strtok(NULL, " \n"); /* getting whatever is after the label */
                if (word == NULL) /* there is nothing after the label */
                {
                    printf("\nError in file %s in line %u, there is nothing after the label definition\n", filename,
                           counter);
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
              if(label_found==1)
              {
                  if(modify==0)  /* need to add a newnode */
                  {
                      if (add_symnode(symhead, label_name, &curnode) == 1)
                      {
                          exit(1);
                      }
                  }
                  else /* find the node and modify it */
                  {
                      findnode(symhead, &curnode, label_name);
                  }
                  curnode->isic=1;
                  curnode->icadrs = *IC;
                  curnode->isinternal = 1;
                  label_found=0;
                  modify=0;
              }

              if(command == RTS || command == STOP) /* if the command doesn't need operands */
                  {
                    (*IC)++;
                    word = strtok(NULL," \n"); /* checking that there is no extra text */
                    if(word != NULL)
                    {
                        printf("\nError in file %s in line %u, extra text after the command: %s\n",filename,counter,word);
                        errors=1;
                        continue;
                    }
                } /* end of if command needs no operands */


                if(command == NOT || command == CLR || command == INC || command == DEC ||
                   command == JMP || command == BNE || command == RED || command == PRN ||
                   command == JSR)                                  /* if the command needs one operand */
                {
                    (*IC)++;
                    word = strtok(NULL," \n"); /* getting the operand */
                    if(word==NULL) /* if the operand is missing */
                    {
                        printf("\nError in file %s in line %u, missing operand\n",filename,counter);
                        errors=1;
                        continue;
                    }

                    if(whataction(word) != -1) /* checking that the operand is not a reserved word */
                    {
                        printf("\nError in file %s in line %u, operand is a reserved word\n", filename,counter);
                        errors=1;
                        continue;
                    }

                    optype = check_operand(word,  filename, counter, IC);   /* getting the operand type */
                    if(optype==-1) /* errors were found */
                    {
                        errors=1;
                        continue;
                    }

                    word = strtok(NULL, " \n"); /* getting whatever is after the operand */
                    if (word != NULL) /* there is nothing after the operand */
                    {
                        printf("\nError in file %s in line %u, extra text after the operand: %s\n", filename, counter,word);
                        errors=1;
                        continue;
                    }

                    if((*tpointer)[command].dest[optype]==0) /* checking if the operand type is valid for the command */
                    {
                        printf("\nError in file %s in line %u, invalid operand type for the command\n",filename,counter);
                        errors=1;
                        continue;
                    }

                } /* end of if the command needs 1 operand */


                if(command == MOV || command == LEA || command == CMP || command == ADD || command == SUB)  /* the command needs 2 operands */
                {
                    (*IC)++;
                    word = strtok(NULL, "\n"); /* getting the every thing after the command */
                    if (word == NULL)
                    {
                        printf("\nError in file %s in line %u, missing operand\n", filename, counter);
                        errors = 1;
                        continue;
                    }
                    strcpy(temparr, word);
                    if (check_commas(temparr, filename, counter) == 1) /* checking the number of commas */
                    {
                        errors = 1;
                        continue;
                    }
                    if (get_ops(temparr, filename, counter, op1, op2) == 1) /* getting the operands into op1 and op2 */
                    {
                        errors = 1;
                        continue;
                    }
                    if (whataction(op1) != -1 || whataction(op2) != -1) /* one of the operands is a reserved word */
                    {
                        printf("\nError in file %s in line %u, operand is a reserved word\n", filename, counter);
                        errors = 1;
                        continue;
                    }
                    op1type = check_operand(op1,  filename, counter, IC);
                    if(op1type==-1)
                    {
                        errors=1;
                        continue;
                    }
                    op2type = check_operand(op2,  filename, counter, IC);
                    if(op2type==-1)
                    {
                        errors=1;
                        continue;
                    }

                    if ((*tpointer)[command].source[op1type] == 0) /* checking the first operand */
                    {
                        printf("\nError in file %s in line %u, source operand type doesn't fit the command: %s\n", filename,counter,op1);
                        errors = 1;
                        continue;
                    }
                    if ((*tpointer)[command].dest[op2type] == 0) /* checking the second operand */
                    {
                        printf("\nError in file %s in line %u, destination operand type doesn't fit the command: %s\n", filename,counter,op2);
                        errors = 1;
                        continue;
                    }
                    if (op1type == REGISTER && op2type == REGISTER) /* if there are 2 registers, they are sharing the same word */
                    {
                        (*IC)--;
                    }
                } /* end of the command needs two operands */


            } /* end of if command is an action */


            if(command>=DATA && command<=EXTERN) /* the command is a direction */
            {
                if(command == EXTERN || command==ENTRY)
                {
                    if(label_found==1)
                    {
                        printf("\nWarning in file %s in line %u, the label %s is redundant and will be ignored\n", filename, counter,label_name);
                        label_found=0;
                    }
                    word = strtok(NULL, " \n"); /* getting the next word */

                    status=analyze_dir(word, counter, filename, symhead,command);
                    if(status==1)
                    {
                        errors = 1;
                        continue;
                    }
                    if(status==-1) /* got memory failure from analyze_dir */
                    {
                        errors=1;
                        return 1;
                    }
                }

                if(command == DATA || command == STRING)
                {
                    if(label_found==1)
                    {
                        if(modify==0)  /* need to add a new node */
                        {
                            if (add_symnode(symhead, label_name, &curnode) == 1)
                            {
                                exit(1);
                            }
                        }
                        else /* find the node and modify it */
                        {
                            findnode(symhead, &curnode, label_name);
                        }
                        curnode->isdc=1;
                        curnode->dcadrs = *DC;
                        curnode->isinternal = 1;
                        label_found=0;
                        modify=0;
                    }

                    if(command==DATA)
                    {
                        if(check_data(filename,counter,DC)==1)
                        {
                            errors=1;
                            continue;
                        }
                    }
                    if(command==STRING)
                    {
                        if(check_string(filename,counter,DC)==1)
                        {
                            errors=1;
                            continue;
                        }
                    }

                }

            } /* end of if command is a direction */

            if(command==-1) /* unknown command */
            {
                printf("\nError in file %s in line %u, unknown command %s\n",filename,counter,word);
                errors=1;
                continue;
            }

            word = strtok(NULL," \n");  /* checking for extra text after the last operand */
            if(word != NULL)
            {
                printf("\nError in file %s in line %u, extra text\n",filename,counter);
                continue;
            }

    } /* no more lines from the file */


    updatedc(symhead,IC);
    if((*IC)+(*DC)-1>256) /*  checks the memory size*/
    {
        printf("\nError in file %s, total size of the memory is bigger than 256 words\n",filename);
        errors=1;
    }

    if(check_entry_exits(symhead,filename)==1) /* checks if entry labels were defined */
    {
        errors=1;
    }

    if(errors==1)
    {
        rewind(*source);
        return 1;
    }
    rewind(*source);

    return 0;
}


/* get_ops copies the operands into op1 and op2  */
int get_ops(char* word,char* filename,int counter,char* op1,char* op2)
{
    int i;
    int j;
    if(word[0]==',')
    {
        printf("\nError in file %s in line %u, there is a comma before the first operand\n",filename,counter);
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
            printf("\nError in file %s in line %u, no comma between operands\n",filename,counter);
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
        printf("\nError in file %s in line %u, second operand missing\n",filename,counter);
        return 1;
    }
    while(word[i] != '\0') /* checking for extra text */
    {
        if(word[i] != ' ' && word[i] != '\t')
        {
            printf("\nError in file %s in line %u, extra text after the second operand\n",filename,counter);
            return 1;
        }
        i++;
    }

    return 0;
}



/* this function checks if .entry or .extern lines are valid.
 * first it checks if the label after the direction is valid.
 * then, depending on the direction, it checks for errors. */
int analyze_dir(char* word,int counter, char* filename, symnode* symhead,int command)
{
    int status; /* used to get output from the functions */
    int placeholder; /* created to use islabel */

    if(word==NULL) /* if there is nothing after .extern or .entry */
    {
        printf("\nError in file %s in line %u, no definition after direction command\n",filename,counter);
        return 1;
    }

    if(islabel(word,counter,filename,&placeholder)==1) /* if word is not a valid label */
    {
        printf("\nError in file %s in line %u, invalid label detected\n",filename,counter);
        return 1;
    }
    if(check_name(word)==1) /* checking if the label is not a reserved word */
    {

        printf("Error in file %s in line %u, the label is a reserved word\n",filename,counter);
        return 1;

    }
    if(command==EXTERN)
    {
        status=check_extern(word, symhead, filename, counter); /* checking if the label is in the list */
        if (status==1)
        {
            return 1;
        }
        if(status==-1) /* memory fail */
        {
            exit(1);
        }
    }
    if(command==ENTRY)
    {
        status=check_entry(word,symhead,filename,counter); /* checking if the label is in the list */
        if(status==1)
        {
            return 1;
        }
        if(status==-1) /* memory fail */
        {
            exit(1);
        }
    }
    word = strtok(NULL," \n"); /* checking for extra text */
    if(word != NULL)
    {
        printf("\nError in file %s in line %u, extra text after the label: %s\n",filename,counter,word);
        return 1;
    }
    return 0;
}


/* this function is used when we detected a command that needs only one operand.
 * this function checks the operand type. */
int check_operand(char *word, char *filename, int counter, int *IC)
{
    int placeholder=0; /* was created so that we can use the function islabel */
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
            (*IC)++;
            return REGISTER;
        }
    }

    if(word[size-1] == ')' )      /* checking if it's a jump operand */
    {
        if(strchr(word,'(') != NULL)
        {
            if(check_jump(word,IC,filename,counter)==0)
            {
                return JUMP;
            }
            else
            {
                printf("\nError in file %s in line %u, invalid operand %s\n",filename,counter,word);
                return -1;
            }
        }
    }

    if(word[0]=='#') /* there might be a number */
    {
        if(word[1] != '+' && word[1] != '-' && isdigit(word[1])==0)
        {
            printf("\nError in file %s in line %u, invalid definition of operand of immediate type\n",filename,counter);
            return -1;
        }

        if( isdigit(word[1]) || word[1]=='+' || word[1]=='-') /* after # there might be a number */
        {
            if(isdigit(word[1]) || word[1]=='-' || word[1]=='+')
            {
                arr[0]=word[1];
            }
            for(x=1,j=2;j<size;j++,x++)
            {
                if(isdigit(word[j])==0)
                {
                    printf("\nError in file %s in line %u, didn't receive a valid number\n",filename,counter);
                    return -1;
                }
                arr[x]=word[j];
            }
            arr[x]='\0';
            if(atoi(arr)>2047)
            {
                printf("\nError in file %s in line %u, number is bigger than 2047\n",filename,counter);
                return -1;
            }
            if(atoi(arr)<-2047)
            {
                printf("\nError in file %s in line %u, number is smaller than -2047\n",filename,counter);
                return -1;
            }
        }
        (*IC)++;
        return IMMEDIATE;
    }

    /* checking if the operand is a label */

    if(islabel(word,counter,filename,&placeholder)==0) /* if the word is a label definition */
    {
        if (check_name(word) == 1) /* checking if the label is not a reserved word */
        {
            printf("Error in file %s in line %u, the label is a reserved word\n", filename, counter);
            return -1;
        }
        (*IC)++;
        return DIRECT;
    }


    printf("\nError in file %s in line %u, invalid operand: %s\n",filename,counter,word);
    return -1;

}

/* this function checks if the operand of jump type is without error
 * first it checks if the label to be jumped to is valid.
 * then it checks the first parameter and the second parameter. */
int check_jump(char* word, int* IC,char* filename,int counter)
{
    int placeholder=0; /* created so that we can use islabel */
    char par1[MAX_LINE]; /* holds the first parameter */
    char par2[MAX_LINE]; /* holds the second parameter */
    char label[MAX_LINE]; /* holds the label */
    char* lpointer=label;
    int i=0;
    int j=0;
    int t=0;
    int regnum=0; /* will keep track of how many register there are */

    while(word[i] != '(') /* getting the label */
    {
        label[i]=word[i];
        i++;
    }
    label[i]='\0';
    if(label[0]=='\0') /* missing label to be jumped to*/
    {
        printf("\nError in file %s in line %u, missing label to be jumped to\n",filename,counter);
        return 1;
    }

    if(islabel(lpointer,counter,filename,&placeholder)==0) /* if the word is a label definition */
    {
        if (check_name(label) == 1) /* checking if the label is not a reserved word */
        {
            printf("\nError in file %s in line %u, the label of the operand is invalid\n",filename,counter);
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
        printf("\nError in file %s in line %u, missing comma in the jump operand\n",filename,counter);
        return 1;
    }
    if(par1[0]=='\0') /* missing parameter */
    {
        printf("\nError in file %s in line %u, missing parameter\n",filename,counter);
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
        printf("\nError in file %s in line %u, missing parameter\n",filename,counter);
        return 1;
    }

    /* checking parameters */

    if(check_parameter(par1,filename,counter,&regnum)==-1)
    {
        return 1;
    }
    if(check_parameter(par2,filename,counter,&regnum)==-1)
    {
        return 1;
    }

    if(regnum==2) /* if there are 2 registers in the jump operand */
    {
        (*IC)++;
    }
    else
    {
        (*IC)=(*IC)+2;
    }

    return 0;
}

/* this function is used to check the parameter in a jump operand. */
int check_parameter(char* word,char* filename,int counter,int* regnum)
{
    int status; /* is used to get output from functions */
    int placeholder=0; /* is created so we can use islabel */

    if(whataction(word) != -1) /* checking that the parameter is not a reserved word */
    {
        printf("\nError in file %s in line %u,parameter is a reserved word\n",filename,counter);
        return -1;
    }
    if(isregister(word)==0) /* checking if the parameter is a register */
    {
        (*regnum)++;
        return REGISTER;
    }
    if((status=isnum(word,filename,counter))==0) /* checking if the parameter is a number */
    {
        return IMMEDIATE;
    }
    if(status==-1) /* isnum returned an error */
    {
        return -1;
    }
    if((status=islabel2(&word,counter,filename,&placeholder))==0)
    {
        return DIRECT;
    }
    if(status==1) /*islabel returned an error */
    {
        return -1;
    }
    return -1;
}






