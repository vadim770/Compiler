#include "symblist.h"

/* this function adds a label to the symbol list
 * and  points to that added node with curnode */
int add_symnode(symnode* head,char* name,symnode* curnode)
{
    symnode newnode;
    symnode p1=*head;
    newnode = (symnode)malloc(sizeof (snode));
    if(newnode==NULL)
    {
        printf("\nError, memory allocation failed\n");
        return 1;
    }

    newnode->next=NULL;
    strcpy(newnode->symname,name);
    newnode->isic=0;
    newnode->isdc=0;
    newnode->isinternal=0;
    newnode->isextern=0;
    newnode->isentry=0;
    newnode->icadrs=0;
    newnode->dcadrs=0;
    newnode->found=0;


    if(!(*head))
    {
        (*head)=newnode;
        (*curnode)=newnode;
        return 0;
    }

    while(p1 != NULL)
    {
        if(p1->next==NULL)
        {
            p1->next=newnode;
            (*curnode)=newnode;
            return 0;
        }
        p1=p1->next;
    }

    return 0;
}

/* this function goes over the symbol list and
 * updates the DC address of the data nodes. */
void updatedc(symnode* head,int* IC)
{

    symnode temp=*head;
    while(temp != NULL)
    {
        if(temp->isdc==1)
        {
            temp->dcadrs=(*IC)+temp->dcadrs;

        }
        temp=temp->next;
    }

}

/* this function checks that labels that were declared as entry were defined in the file */
int check_entry_exits(symnode* head,char* filename)
{
    int errors=0;
    symnode temp=*head;
    while(temp != NULL)
    {
        if(temp->isentry==1 && temp->isinternal==0)
        {
            printf("\nError in file %s, label %s was declared as entry but was not defined\n",filename,temp->symname);
            errors =1;
        }
        temp=temp->next;
    }
    if(errors==1)
    {
        return 1;
    }
    return 0;
}

/* this function frees the symbol list */
void free_symlist(symnode* head)
{
    symnode p1;
    while(*head != NULL)
    {
        p1=*head;
        (*head)=(*head)->next;
        free(p1);
    }
}


/* findnode searches for a node is the symbol list.
 * curnode will point to it.*/
void findnode(symnode* head, symnode* curnode,char* name)
{
    symnode temp =*head;

    while(temp != NULL)
    {
        if(strcmp(name,temp->symname)==0)
        {
            *curnode=temp;
            return;
        }
        temp=temp->next;
    }
}

/* the function compares word with the label name from the label list */
int check_lists(char* word, symnode* symname,char* filename, int counter,int* modify)
{

    symnode temp = *symname;

    while(temp != NULL)
    {
        if(strcmp(word,temp->symname)==0)
        {
            if(temp->isextern==1)
            {
                printf("\nError in file %s in line %u, label %s was already defined as external\n",filename,counter,word);
                return 1;
            }
            if(temp->isentry==1 && temp->isinternal==1)
            {
                printf("Error in file %s in line %u, the label %s was already defined\n", filename, counter,word);
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
                printf("Error in file %s in line %u, the label %s was already defined\n", filename, counter,word);
                return 1;
            }

        }
        temp=temp->next;
    }
    return 0;
}

/* this function checks that the label after the .extern direction
 * wasn't declared as internal or entry and that it was declared once. */
int check_extern(char* word, symnode* symhead,char* filename, int counter)
{
    symnode head = *symhead;

    while(head != NULL)
    {
        if(strcmp(word,head->symname)==0)
        {
            if(head->isinternal==1) /* if the label was already defined in the file */
            {
                printf("\nError in file %s in line %u, external label %s was already defined as internal\n",filename,counter,word);
                return 1;
            }
            if(head->isextern==1) /* if the label was already declared as extern */
            {
                printf("\nError in file %s in line %u, external label %s was already declared\n",filename,counter,word);
                return 1;
            }
        }
        head=head->next;
    }
    /* the label is not in the list */
    head=NULL;
    if(add_symnode(symhead,word,&head)==1)
    {
        printf("\nError, memory allocation failed\n");
        return -1;
    }
    head->isextern=1;
    head->isinternal=0;
    head->isentry=0;
    head->found=0;
    return 0;

}

/* this function checks if a label that was found after a .entry definition
 * is not external and that it was declared once an entry */
int check_entry(char* word, symnode* symhead,char* filename, int counter)
{
    symnode head = *symhead;

    while(head != NULL)
    {
        if(strcmp(word,head->symname)==0)
        {
            if(head->isextern==1)
            {
                printf("\nError in file %s in line %u, the label %s was already defined as external\n",filename,counter,word);
                return 1;
            }
            if(head->isentry==1)
            {
                printf("\nError in file %s in line %u, the label %s was already declared as entry\n",filename,counter,word);
                return 1;
            }
            if(head->isinternal==1)
            {
                head->isentry=1;
                return 0;
            }
            head->isentry=1;
        }
        head=head->next;
    }
    /* if the label is not in the list */
    head=NULL;
    if(add_symnode(symhead,word,&head)==1)
    {
        printf("\nError, memory allocation failed\n");
        return -1;
    }
    head->isextern=0;
    head->isinternal=0;
    head->isentry=1;
    return 0;
}

/* this function fills the .ent file with entry labels */
void fill_entry(symnode* head,FILE** entfile)
{
    symnode temp = *head;
    while(temp != NULL)
    {
        if(temp->isentry==1)
        {
            if(temp->isic==1)
            {
                fprintf(*entfile,"%s %d\n",temp->symname,temp->icadrs);

            }
            if(temp->isdc==1)
            {
                fprintf(*entfile,"%s %d\n",temp->symname,temp->dcadrs);

            }
        }
        temp=temp->next;
    }
}

/* this function checks that all the external labels were used in the code */
int check_extern_nodes(symnode* head,char* filename)
{
    symnode temp = *head;
    int errors=0;

    while(temp != NULL)
    {
        if(temp->isextern==1 && temp->found==0) /* found a label the is never used */
        {
            printf("\nError in file %s, external label %s is never used\n",filename,temp->symname);
            errors=1;
        }
        temp=temp->next;
    }
    if(errors==1)
    {
        return 1;
    }
    return 0;
}

/* this function was called if an operand is a label.
 * the function checks if the label was defined and if it is external or internal. */
int get_address(symnode* head,char* word,int counter, char* filename,FILE** extfile,int* IC)
{
    symnode temp = *head;

    while(temp != NULL)
    {
        if(strcmp(word,temp->symname)==0)
        {

            if(temp->isextern==1) /* if the label is external */
            {
                temp->found=1; /* flagging that it was found */
                fprintf(*extfile,"%s   %d\n",word,*IC);
                return -1;
            }
            if(temp->isic==1)
            {
                return temp->icadrs;
            }
            if(temp->isdc==1)
            {
                return temp->dcadrs;
            }
        }
        temp=temp->next;
    }
    printf("\nError in file %s in line %u, the label %s was not referenced\n",filename,counter,word);
    return -2;
}

