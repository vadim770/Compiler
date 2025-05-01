#include "macrolist.h"

/* goes over the list and frees all the list*/
void free_macro_list(mpointer* head)
{
    mpointer p1;
    while(*head != NULL)
    {
        p1=*head;
        *head = (*head)->next;
        free(p1->macroname);
        free(p1->macro);
        free(p1);
    }
}

/* adds a new node to the macro list */
int add_mnode(mpointer* head, char* mname, char* mac)
{
    mpointer newnode;
    newnode = (mpointer)malloc(sizeof (mnode));
    if(newnode==NULL)
    {
        printf("\nError, memory allocation failed\n");
        return 1;
    }
    newnode->macroname=(char*)malloc((strlen(mname)+1)*sizeof(char));
    if(newnode->macroname==NULL)
    {
        printf("\nError, memory allocation failed\n");
        free(newnode);
        return 1;
    }
    newnode->macro=(char*)malloc((strlen(mac)+1)*sizeof(char));
    if(newnode->macro==NULL)
    {
        printf("\nError, memory allocation failed\n");
        free(newnode->macroname);
        free(newnode);
        return 1;
    }

    strcpy(newnode->macroname,mname);
    strcpy(newnode->macro,mac);

    if(!(*head))
    {
        (*head)=newnode;
        newnode->next=NULL;
        return 0;
    }

    newnode->next=(*head);
    (*head)=newnode;
    return 0;
}



