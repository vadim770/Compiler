#include "firstpass_input_check.h"

/* word might contain a valid label
 * the function checks if word is a valid label */
int islabel(char* word,int counter,char* filename, int* errors)
{
    int i;
    int size = strlen(word);

    if(size>30)
    {
        printf("\nError in file %s in line %u, label name is bigger than 30 characters\n",filename,counter);
        *errors=1;
        return 1;
    }
    if(isalpha(word[0])==0) /* if the first char is not a letter */
    {
        printf("\nError in file %s in line %u, the label %s does not start with a letter\n",filename,counter,word);
        *errors=1;
        return 1;
    }
    for(i=1;i<size;i++) /* checking the other chars of the label */
    {
        if(isalnum(word[i])==0)
        {
            printf("\nError in file %s in line %u, the label %s must consist of only letters and numbers\n",filename,counter,word);
            *errors=1;
            return 1;
        }
    }

    return 0;
}

/* this function checks if word is a register */
int isregister(char* word)
{
    int i;
    char* saved_name[8]; /* creating an array with all the reserved words */
    saved_name[0]="r0";
    saved_name[1]="r1";
    saved_name[2]="r2";
    saved_name[3]="r3";
    saved_name[4]="r4";
    saved_name[5]="r5";
    saved_name[6]="r6";
    saved_name[7]="r7";

    for(i=0;i<8;i++)
    {
        if(strcmp(word,saved_name[i])==0)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * this function checks if word is a number */
int isnum2(char* word,char* filename,int counter)
{
    int j;
    int size = strlen(word);

    if( isdigit(word[0]) || word[0]=='+' || word[0]=='-')
    {
        for(j=1;j<size;j++)
        {
            if(isdigit(word[j])==0)
            {
                printf("\nError in file %s in line %u, didn't receive a valid number\n",filename,counter);
                return 1;
            }
        }
        if(atoi(word)>2047)
        {
            printf("\nError in file %s in line %u, number is bigger than 8191\n",filename,counter);
            return -1;
        }
        if(atoi(word)<-2047)
        {
            printf("\nError in file %s in line %u, number is smaller than -8192\n",filename,counter);
            return -1;
        }

        return 0;
    }
    printf("\nError in file %s in line %u, didn't receive a valid number\n",filename,counter);
    return 1;
}

/* this function checks if word is a valid number */
int isnum(char* word,char* filename,int counter)
{
    int j;
    int i;
    char arr[MAX_LINE];
    int size = strlen(word);

    if(word[0]=='#') /* there might be a number */
    {
        if(word[1] != '+' && word[1] != '-' && isdigit(word[1])==0)
        {
            printf("\nError in file %s in line %u, invalid definition of operand of immediate type\n",filename,counter);
            return -1;
        }

        if( isdigit(word[1]) || word[1]=='+' || word[1]=='-') /* after # there might be a number */
        {
            if(isdigit(word[1])  || word[1]=='-' || word[1]=='+')
            {
                arr[0]=word[1];
            }

            for(i=1,j=2;j<size;j++,i++)
            {
                if(isdigit(word[j])==0) /* if we encountered a non digit char */
                {
                    printf("\nError in file %s in line %u, didn't receive a valid number\n",filename,counter);
                    return -1;
                }
                arr[i]=word[j];
            }
            arr[i]='\0';
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

            return 0;
        }
    }
    return 1;
}

/* this function checks if word is a label. */
int islabel2(char** word,int counter,char* filename, int* errors)
{
    int i;
    int size = strlen(*word);

    if(isalpha((*word)[0])==0) /* if the first char is not a letter */
    {
        *errors =1;
        return 1;
    }
    for(i=1;i<size;i++) /* checking the other chars of the label */
    {
        if(isalnum((*word)[i])==0)
        {
            return 1;
        }
    }
    if(size>30)
    {
        printf("\nError in file %s in line %u, label name is bigger than 30 character\n",filename,counter);
        return 1;
    }

    return 0;
}

/* this function checks if there is a comma that separates the operands */
int check_commas(char* word,char* filename,int counter)
{
    char* com1 = strchr(word,',');
    char* com2 = strrchr(word,',');

    if(com1 == NULL && com2 == NULL) /* there are no commas */
    {
        printf("\nError in file %s in line %u, there are no commas\n",filename,counter);
        return 1;
    }
    if(com1==com2) /* com1 and com2 point to the same comma */
    {
        return 0;
    }
    printf("\nError in file %s in line %u, multiple commas\n",filename,counter);
    return 1;
}

/* this function will be used after getting a .string direction.
 * the function will start by checking if there are two " chars.
 * it then checks if there is no text between the .string direction and the first ".
 * once getting to the first " the function copies every thing until reaching the next " char.
 * next it will check that there is no text after the second ".
 * it will update DC accordingly */
int check_string(char* filename,int counter, int* DC)
{
    char* word; /* will hold the string to be checked */
    int i;
    int j=0;
    char arr[MAX_LINE]; /* will hold the string in "" */
    char* p1; /* will point to the first " */
    char* p2; /* will point to the last " */

    word = strtok(NULL,"\n"); /* getting every thing after the .string direction */

    if(word==NULL)
    {
        printf("\nError in file %s in line %u, empty line after .string\n",filename,counter);
        return 1;
    }

    p1 = strchr(word,'\"');
    p2 = strrchr(word,'\"');

    if(p1==NULL || p2==NULL || p1==p2) /* if there is no quotation mark or one is missing */
    {
        printf("\nError in file %s in line %u, missing quotation mark in string definition\n",filename,counter);
        return 1;
    }

    for(i=0; word[i] != '\"'; i++) /* checking for text before the string */
    {
        if(word[i] != ' ' && word[i] != '\t')
        {
            printf("\nError in file %s in line %u, there is text before the string\n",filename,counter);
            return 1;
        }
    }

    for(i=i+1; word[i] != '\"' ; i++,j++) /* getting the string in " " */
    {
        arr[j]=word[i];
    }
    arr[j]='\0';

    if(arr[0]=='\0') /* the string is empty */
    {
        printf("\nError in file %s in line %u, the string is empty\n",filename,counter);
        return 1;
    }

    (*DC)=(*DC)+ strlen(arr)+1;

    for(i=i+1; word[i] != '\0' ; i++) /* checking for extra text after the last "  */
    {
        if(word[i] != ' ' && word[i] != '\t')
        {
            printf("\nError in file %s in line %u, extra text after the string\n",filename,counter);
            return 1;
        }
    }
    return 0;
}

/* check_data is used after getting the .data direction
 * it will check if the input of the user is valid
 * on the first loop the function will look for a valid number
 * on later loops it will look for a comma, and after the comma a valid number */
int check_data(char* filename,int counter,int *DC)
{
    char* word = strtok(NULL,"\n"); /* getting the rest of the line */
    int i=0;
    int j=0;
    int first_loop=1; /* a flag the signals that this is the first loop */
    int comma_found=0; /* a flag the signals that a comma was found */
    int num_found=0; /* a flag the signals that a number was found */
    char arr[MAX_LINE]; /* arr will hold a potential number, the content of arr will be checked */

    if(word == NULL) /* if there is nothing after the .data direction */
    {
        printf("\nError in file %s in line %u, empty line after .data\n",filename,counter);
        return 1;
    }

    while(word[i] != '\0')
    {
        if(word[i] == '\t' || word[i] == ' ') /* skip white space */
        {
            i++;
            continue;
        }

        if(word[i] != ',') /*found the start of a string */
        {
            if(first_loop==0)
            {
                if(comma_found==0) /* there is no comma before a number */
                {
                    printf("\nError in file %s in line %u, missing comma before a number\n",filename,counter);
                    return 1;
                }
                if(num_found==1)
                {
                    printf("\nError in file %s in line %u, missing comma between two numbers\n",filename,counter);
                    return 1;
                }
            }
            if(first_loop==1)
            {
                first_loop=0;
            }
            while(word[i] != '\0' && word[i] != ',' && word[i] != ' ' && word[i] != '\t') /* getting a string without white space and a comma */
            {
                arr[j]=word[i];
                i++;
                j++;
            }
            arr[j]='\0';
            j=0;
            num_found=1;
            comma_found=0;
            if(isnum2(arr,filename,counter)==1) /* the number is invalid */
            {
                return 1;
            }
            (*DC)++;

        } /* end of finding a string */

        if(word[i] == ',')
        {
            if(comma_found==1)
            {
                printf("\nError in file %s in line %u, multiple commas detected\n",filename,counter);
                return 1;
            }
            if(num_found==0)
            {
                printf("\nError in file %s in line %u, missing number before a comma\n",filename,counter);
                return 1;
            }
            comma_found=1;
            num_found=0;
            i++;
        }

    } /* end of analyzing the line */
    if(comma_found==1)
    {
        printf("\nError in file %s in line %u, missing number after comma\n",filename,counter);
        return 1;
    }
    return 0;
}

