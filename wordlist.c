#include "wordlist.h"



/* adds a memory word to the list  */
int add_word_node(mempointer* head,int* DC,mempointer* curnode)
{
    mempointer newnode;
    mempointer p1=*head;
    newnode = (mempointer)malloc(sizeof (wnode));
    if(newnode==NULL)
    {
        printf("\nError, memory allocation failed\n");
        return 1;
    }

    newnode->next=NULL;
    newnode->dc=*DC;

    initialize_memword(newnode->memword);

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

/* this function frees the memory list */
void free_word_list(mempointer* head)
{
    mempointer p1;
    while(*head != NULL)
    {
        p1=*head;
        (*head)=(*head)->next;
        free(p1);
    }
}

/* this function goes over the memory list and prints node by node their values to the .ob file */
void print_data(mempointer* head,int* IC, FILE** obfile)
{
    mempointer temp=*head;
    int address;

    while(temp != NULL)
    {
        address=(*IC)+temp->dc;
        print_word(temp->memword,obfile,&address);
        temp=temp->next;
    }
}