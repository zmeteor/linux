#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void mylink_init(mylink *head)	
{
    *head = NULL;
}

mylink make_node(char *item, int fd)
{
    mylink p = (mylink)malloc(sizeof(struct node));
    strcpy(p->item,item);
    p->fifo_fd = fd;

    p->next = NULL;				
    return p;
}

void mylink_insert(mylink *head, mylink p)		
{
    p->next = *head;
    *head = p;
}

mylink mylink_search(mylink *head, char *keyname)
{
    mylink p;
    for (p = *head; p != NULL; p = p->next)
    {
        if (strcmp(p->item,keyname) == 0)
        {
            return p;
        }
    }
    return NULL;
}

void mylink_delete(mylink *head, mylink q)
{
    mylink p;
    if (q == *head)
    {
        *head = q->next;
        return;
    }
    for (p = *head; p != NULL; p = p->next)
    {
        if (p->next == q)
        {
            p->next = q->next;
            return;
        }
    }
}

void free_node(mylink p)
{
    free(p);
}

void mylink_destory(mylink *head)
{
    mylink p= *head, q;
    while (p != NULL)
    {
        q = p->next;
        free(p);
        p = q;
    }
    *head = NULL;
}

void mylink_travel(mylink *head, void (*vist)(mylink))
{
    mylink p;
    for (p = *head; p != NULL; p = p->next)
    {
        vist(p);
    }
}
