#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int arrival, duration, memory, priority;
    struct process *next;
} process;

process *createQueue()
{
    process *head = malloc(sizeof(process));
    head->arrival = 0;
    head->duration = 0;
    head->memory = 0;
    head->priority = 0;
    head->next = NULL;
    return head;
}

void main()
{
    process *queue0, *queue1, *queue2, *queue3, *queue4, *queue5;
    queue0 = createQueue();
    queue1 = createQueue();
    queue2 = createQueue();
    queue3 = createQueue();
    queue4 = createQueue();
}