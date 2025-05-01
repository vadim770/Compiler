#include "file_funcs.h"

/* this function checks if the files exists and are not empty.
 * returns 1 if we encounter a problem with a file. */
int check_files(char* filenames, FILE** tempfd)
{
    unsigned long size;                 /* will be used to check the file size */
    char* ext =".as";                   /* a const string that holds the extension that we will look for */
    unsigned long file_name_size;       /* holds the size of the current file name */
    char* name=NULL;                    /* will hold the full file name with the extension */

    file_name_size = strlen(filenames);

    name=(char*)malloc((file_name_size+10) * sizeof(char)); /* the 10 is just to be on the safe side */
    if(!name)
    {
        printf("Error, not enough memory\n");
        exit(1);
    }

    strcpy(name,filenames);
    strcat(name,ext);   /* name now hold the file name with the extension */

    (*tempfd) = fopen(name,"r");
    if (!(*tempfd))
    {
        fprintf(stdout, "\nError, could not open or find file: %s\n",name);

        free(name);
        return 1;
    }

    fseek((*tempfd), 0, SEEK_END);
    size = ftell((*tempfd));
    if(!size) /* if the file is empty */
    {
        fprintf(stdout, "\nError, the file %s is empty\n",name);
        fclose(*tempfd);
        free(name);
        return 1;
    }
    fseek(*tempfd, 0, SEEK_SET);
    free(name);
    return 0;
}


/* this function creates the .ob .ent .ext files
 * it also prints the number of IC and DC lines at that start of the .ob file */
int open_files(FILE** obfile,FILE** entfile, FILE** extfile,char* filename,int* IC, int* DC)
{
    char obname[MAX_FILENAME];
    char entname[MAX_FILENAME];
    char extname[MAX_FILENAME];

    strcpy(obname,filename);
    strcpy(entname,filename);
    strcpy(extname,filename);

    strcat(obname,".ob");
    strcat(entname,".ent");
    strcat(extname,".ext");

    *obfile = fopen(obname,"w+");
    if(!(*obfile))
    {
        printf("\nError, could not make object file\n");
        return 1;
    }
    *entfile = fopen(entname,"w+");
    if(!(*entfile))
    {
        printf("\nError, could not make entry file\n");
        return 1;
    }
    *extfile = fopen(extname,"w+");
    if(!(*extfile))
    {
        printf("\nError, could not make extern file\n");
        return 1;
    }

    fprintf(*obfile,"    %d         %d\n",(*IC)-100,(*DC) );

    return 0;
}

/* this function deletes the .ob .ext .ent files when errors where found */
void delete_files(char* filename, FILE** obfile, FILE** entfile, FILE** extfile)
{
    char obname[MAX_FILENAME];
    char entname[MAX_FILENAME];
    char extname[MAX_FILENAME];

    strcpy(obname,filename);
    strcpy(entname,filename);
    strcpy(extname,filename);

    strcat(obname,".ob");
    strcat(entname,".ent");
    strcat(extname,".ext");

    fclose(*obfile);
    fclose(*entfile);
    fclose(*extfile);

    remove(obname);
    remove(entname);
    remove(extname);

}

/* this function checks if the files are empty.
 * it also closes the files */
int isempty(FILE** file)
{
    long size;
    fseek((*file), 0, SEEK_END);
    size = ftell((*file));
    if(!size) /* if the file is empty */
    {
        fclose(*file);
        return 1;
    }
    fclose(*file);
    return 0;
}

/* this function checks if the files are empty and deletes them if they are */
void delete_empty(char* filename, FILE** entfile, FILE** extfile)
{
    char entname[MAX_FILENAME];
    char extname[MAX_FILENAME];

    strcpy(entname,filename);
    strcpy(extname,filename);

    strcat(entname,".ent");
    strcat(extname,".ext");

    if(isempty(entfile)==1)
    {
        remove(entname);
    }
    if(isempty(extfile)==1)
    {
        remove(extname);
    }
}

