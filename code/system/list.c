#include "list.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

void List_Add(list* l, node* n)
{
    n->next = *l;
    *l = n;
}

void List_Del(list* l, node* n)
{
    node* last_note;
    node* curr_note;

    for (curr_note = *l; curr_note != NULL; curr_note = curr_note->next) {
        if (n == curr_note) {
            if (*l == n) // 要删除的是头部的情况
                *l = n->next;
            else
                last_note->next = n->next;

            return;
        }

        last_note = curr_note;
    }
}

void List_Foreach(list* l, void (*handler)(data))
{
    node* curr_note;

    for (curr_note = *l; curr_note != NULL; curr_note = curr_note->next)
        handler(curr_note->d);
}


