
#include "assembler.h"

/* this program acts as a part of an assembler.
 * that program receives text files that contain assembly code.
 * the program looks mainly for syntax errors and for other errors.
 * first, the program will check if the files exist and are not empty.
 * then the program will spread the macros in the .as files and will create the .am file with the spread macros.
 * it will also remove extra white space.
 * in the first pass, the program will look for the majority of the errors, and it will create a symbol list.
 * in the second pass, the program will look for errors. if no errors were found, it will
 * make the .ob .ent .ext files and will fill them accordingly.
 * if any errors were found, then these files will be deleted.
 *
 * assumptions:
 * the maximum line in the .as file is 80 characters long
 * the maximum file name is 256 characters long
 * when defining a .string data type, the program will consider that the string ends after the second ".
 * macro names will always be surrounded by white space or newline */
int main(int argc,char* argv[])
{
    FILE* fd; /* will point to the source file .as */
    FILE* newfile;  /* will point to the file with spread macros .am */
    int i;
    int IC=100;
    int DC=0;
    int errors; /* a flag that signals if errors were found */
    aop actiontable[NUM_OF_ACTIONS]; /* holds the valid addressing for each action */
    aop* tpointer=actiontable;  /* points to the action table */
    symnode symhead=NULL; /* will point to a list of action symbols */


    for(i=1;i<argc;i++) /* this loop goes over all the files that the program received */
    {
        IC=100;
        DC=0;
        errors=0;

        if(check_files(argv[i], &fd) == 1) /* checking if the file exists and is not empty */
        {
            continue;
        }
        if(preassembler(&fd,argv[i],&newfile)==1) /* looking for macros in the source file and creating a new file with spread macros */
        {
            continue;
        }
        if(firstpass(&IC,&DC,&newfile,&symhead,&tpointer,argv[i])==1)
        {
            error_check(&IC,&DC,&newfile,&symhead,argv[i]); /* only checking for errors */
        }
        else
        {
            secondpass(&IC,&DC,&newfile,&symhead,argv[i],errors);
        }


        fclose(fd); /* closing the .as file */
        fclose(newfile); /* closing the .am file */
        if(symhead != NULL)
        {
            free_symlist(&symhead);
        }
    }

    return 0;
}
