#include "preassembler.h"


/* this function goes over the source file and looks for macros
 * when a macro definition was found without errors, it is added to the macro list
 * it will then create a new file and will spread the macros there.
 * it will also get rid of empty lines, comment lines, extra white space in lines .*/
int preassembler(FILE** tempfd, char* filename, FILE** newfile)
{
    mpointer head=NULL; /* holds the macro list */

    if(find_macros(tempfd,&head,filename)==1) /* if there was an error when looking for macros */
    {
        if(head != NULL)
        {
            free_macro_list(&head);
        }
        return 1;
    }


    if(spread_macro(tempfd,newfile,filename,&head)==1) /* creating a new file with the spread macros */
    {
        if(head != NULL)
        {
            free_macro_list(&head);
        }
        return 1;
    }

    if(head != NULL)
    {
        free_macro_list(&head);
    }
    rewind(*tempfd);
    rewind(*newfile);
    return 0;
}

/* this function creates a new .am file and spreads the macros in it.
 * it will also get rid of empty lines, comment lines, extra white space in lines.*/
int spread_macro(FILE** source,FILE** newfile,char* filename,mpointer* head)
{
    char* newfilename; /* will hold the new file name with .am extension */
    char line[MAX_LINE]; /* holds a line from the source file */
    char* word; /* holds the first word in the line */
    mpointer pointer; /* pointer will be used to search the macro list */
    int found; /* a flag that signals if we found a macro in a line */

    newfilename=(char*)malloc((strlen(filename)+10)*sizeof(char)); /* the 10 is just to be safe */
    if(newfilename==NULL)
    {
        printf("\nError, memory allocation failed\n");
        fclose(*source);
        return 1;
    }

    strcpy(newfilename,filename);
    strcat(newfilename,".am"); /* newfilename now holds the file name with .am extension */

    *newfile = fopen(newfilename,"w+"); /* creating a new .am file */
    if(!(*newfile))
    {
        printf("\nError, could not create file\n");
        exit(1);
    }
    rewind(*source);

    while( fgets(line,MAX_LINE,*source) != NULL)
    {
        word = strtok(line," \t\n");
        if(word==NULL || strcmp(word,"\r")==0 ) /* if word is newline */
        {
            continue;
        }
        if(word[0]==';') /* if the first char is ; */
        {
            continue;
        }
        if(strcmp(word,"mcr")==0) /* if word is mcr */
        {
            while(word==NULL || strcmp(word,"endmcr") != 0) /* skips lines until we encounter endmcr */
            {
                fgets(line,MAX_LINE,*source);
                word = strtok(line," \t\n");
            }
            continue;
        }
        while(word != NULL ) /* if word is not newline and not mcr */
        {
            pointer = *head;
            found=0;
            while (pointer != NULL) /* checking if the word is a macro name */
            {
                if (strcmp(word, pointer->macroname) == 0) /* if word is a macro name */
                {
                    put_macro(pointer->macro,newfile); /* put the macro */


                    found=1;
                    break;
                }
                pointer = pointer->next;
            }
            if(found==1)
            {
                word = strtok(NULL," \t\n");
            }
            else /* the word is not a macro name */
            {
                fputs(word,*newfile);
                fputc(' ',*newfile);
                word = strtok(NULL," \t\n");
            }
        }
        fprintf(*newfile,"\n");
    }

    free(newfilename);
    return 0;
}

/* this function gets the macro from the macro list
 * and puts it in the .am file.
 * the function will get rid of extra white space */
void put_macro(char* macro,FILE** newfile)
{
    int i=0;
    int word_found=0; /* a flag that signals that a word was found */

    while(macro[i] != '\0')
    {
        while(macro[i] == ' ' || macro[i] == '\t' ||  macro[i] == '\n') /* skip white space */
        {
            if(macro[i]=='\n')
            {
                if(word_found==1) /* print '\n' only after a word */
                {
                    fputc('\n',*newfile);
                }
                word_found=0;
            }
            i++;
        }

        while(macro[i] != ' ' && macro[i] != '\t'  &&  macro[i] != '\n' && macro[i] != '\0') /* print non white space */
        {
            fputc(macro[i],*newfile);
            i++;
            word_found=1;
        }
        if(macro[i]==' ' || macro[i] == '\t') /* if the word ended with space */
        {
            fputc(' ',*newfile);
        }
    }

    if(word_found == 0) /* if there is only white space after the last word */
    {
        fseek(*newfile,-1,SEEK_CUR); /* this line deletes the '\n' that will be put later */
    }

}

/* this function goes over the .as file and looks for macros
 * it goes line bt line. it checks the first word of each line for the start
 * of a macro definition. it then checks if the macro name is valid.
 * then, it uses get_macro to copy the macro into the macro list. */
int find_macros(FILE** tempfd,mpointer* head,char* filename)
{
    char* mstart="mcr";
    char* mname; /*holds the macro name */
    char* mdata;    /* holds the contents of the macro */
    char line[MAX_LINE];    /* holds a line from the file */
    char* word; /* holds a word from the file */
    int counter=0; /* keeps the number of the line that we are in */

    while(fgets(line,MAX_LINE,*tempfd) != NULL)
    {
        mname=(char*)malloc(MAX_LINE*sizeof(char));
        if(!mname)
        {
            printf("\nError, memory allocation failed\n");
            exit(1);
        }
        counter++;
        word = strtok(line," \t\n"); /* getting the first word in the line */
        if(word==NULL)
        {
            free(mname);
            continue;
        }
        if(strcmp(word,mstart) != 0)        /* looking for the start of the macro */
        {
            free(mname);
            continue;
        }
        word = strtok(NULL," \t\n"); /* getting the name of the macro */
        if(word==NULL || check_macro_name(word)==1) /* checking if the macro name is not a reserved word */
        {
            if(word != NULL)
            {
                printf("\nError in file %s, the macro name in line %u is a saved word\n",filename, counter);
            }
            else
            {
                printf("\nError in file %s, macro name is missing in line %u\n",filename, counter);

            }
            fclose(*tempfd);
            free(mname);
            return 1;
        }

        strcpy(mname,word);

        word = strtok(NULL," \t\n"); /* checking for extra text after the macro name */
        if(word != NULL && isspace(word[0])==0)
        {
            printf("\nError in file %s, extra text after macro name in line %u\n",filename, counter);
            fclose(*tempfd);
            free(mname);
            return 1;
        }
        if(get_macro(tempfd, &mdata, &counter, filename) == 1) /* if getting the macro failed */
        {
            free(mname);
            return 1;
        }

        if(add_mnode( head, mname,mdata)==1) /* if adding a node failed */
        {
            free(mname);
            free(mdata);
            exit(1);
        }

        free(mname);
        free(mdata);
    }
    return 0;
}

/* get_macro copies the macro from the .as file into the macro list.
 * the function will look for the position in the file where the macro start and for the
 * position where the macro ends. it will copy it to the macro list*/
int get_macro(FILE **tempfd, char **mdata, int *counter, char *filename)
{
    char line[MAX_LINE]; /* holds a line from the source file */
    char* word; /* holds the first word in the line */
    char* mend="endmcr"; /* this string will be compared to check for the end of a macro definition */
    fpos_t start_pos; /* will hold the starting position of the macro in the source file */
    fpos_t end_pos;     /* will hold the ending position of the macro in the source file */
    fpos_t cont;        /* will hold the position in which we will continue to read from the source file after the macro has ended */
    unsigned int end_size; /* will hold the size in bytes from the start of the source file to the end of the macro */
    unsigned int start_size;  /* will hold the size in bytes from the start of the source file to the start of the macro */
    unsigned int macro_size; /* will hold the size of the macro in bytes */

    fgetpos(*tempfd,&start_pos); /* getting the starting position of the macro */

    fgets(line,MAX_LINE,*tempfd);
    (*counter)++;
    word = strtok(line," \t\n");

    if(strcmp(word,mend)==0)   /* if the macro is empty */
    {
        word = strtok(NULL," "); /* checking for extra text after the end of the macro ending */
        if(word != NULL && isspace(word[0])==0)
        {
            printf("\nError in file %s, extra text after macro name in line %u\n",filename, *counter);
            fclose(*tempfd);
            return 1;
        }

        (*mdata)=(char*)malloc(1*sizeof (char));
        (*mdata)[0]='\0';
        return 0;
    }

    while(word==NULL || strcmp(word,mend) != 0)   /* getting the macro contents, this loop continues as long as word is not endmcr */
    {
        fgetpos(*tempfd,&end_pos);  /* updating the end position */
        fgets(line,MAX_LINE,*tempfd); /* getting a new line from the source file */
        (*counter)++;
        word = strtok(line," \t\n");  /* getting the first word in the line */
    }

    word = strtok(NULL," "); /* checking for extra text after the end of the macro ending */
    if(word != NULL && isspace(word[0])==0)
    {
        printf("\nError in file %s, extra text after macro name in line %u\n",filename, *counter);
        fclose(*tempfd);
        return 1;
    }

    fgetpos(*tempfd,&cont); /* saving the position in which we will continue looking for macros in the source file */
    fsetpos(*tempfd,&end_pos);
    end_size = ftell(*tempfd); /* this is where the macro ends */
    fsetpos(*tempfd,&start_pos);
    start_size = ftell(*tempfd); /* this is where the macro starts */
    macro_size =  end_size  - start_size; /* getting the size of the macro in bytes */
    (*mdata) = (char*)malloc((macro_size+1)*sizeof(char));
    if(!(*mdata))
    {
        printf("\nError, memory allocation failed\n");
        fclose(*tempfd);
        exit(1);
    }

    fread(*mdata,sizeof(char),macro_size-1,*tempfd); /* reading the macro into mdata */
    (*mdata)[macro_size-1]='\0';
    fsetpos(*tempfd,&cont); /* positioning the file to the end of endmcr */
    return 0;
}

/* checks if mname is not a reserved word */
int check_macro_name(char* mname)
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
        if(strcmp(mname,saved_name[i])==0)
        {
            return 1;
        }
    }
    return 0;
}

