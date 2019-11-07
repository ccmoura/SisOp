/*
Escalonador de processos
Autor: Christopher Conceição Moura
Disciplina: Sistemas Operacionais
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    false,
    true
} boolean;
typedef struct node
{
    int arrival, duration, memory, priority, current;
    struct node *next;
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

void enqueueProcess(int array[], process *queue)
{
    process *newProcess = malloc(sizeof(process));
    newProcess->arrival = array[0];
    newProcess->duration = array[1];
    newProcess->memory = array[2];
    newProcess->priority = array[3];
    newProcess->current = 0;
    newProcess->next = queue->next;
    queue->next = newProcess;
}

boolean readProcessesFile(char fileName[], process *queue0, process *queue1, process *queue2, process *queue3, process *queue4)
{
    FILE *f;
    int arrival, duration, memory, priority, i, parametersArray[4];
    char line[50], *result, *token;
    f = fopen(fileName, "r");
    if (f == NULL)
    {
        printf("Error opening %s.\n", fileName);
        return false;
    }
    while (!feof(f))
    {
        result = fgets(line, 50, f);
        if (result)
        {
            i = 0;
            token = strtok(result, ", ");
            while (token != NULL)
            {
                parametersArray[i++] = atoi(token);
                token = strtok(NULL, ", ");
            }
            switch (parametersArray[3])
            {
            case 0:
                enqueueProcess(parametersArray, queue0);
                break;
            case 1:
                enqueueProcess(parametersArray, queue1);
                break;
            case 2:
                enqueueProcess(parametersArray, queue2);
                break;
            case 3:
                enqueueProcess(parametersArray, queue3);
                break;
            case 4:
                enqueueProcess(parametersArray, queue4);
                break;
            }
        }
    }
    fclose(f);
    return true;
}
boolean dequeue(process *queue){
    process *p = queue->next, *q = queue;
    if(p == NULL) return false;
    while(p->next != NULL){
        p = p->next;
        q = q->next;
    }
    q->next = NULL;
    free(p);
    return true;
}
void printQueue(process *queue){
    process *p;
    p = queue->next;
    while(p != NULL){
        printf("%d, %d, %d, %d, %d\n", p->arrival, p->duration, p->memory, p->priority, p->current);
        p = p->next;
    }
}

void run(int cpus, int sliceDuration, int totalMemory, process *queue0, process *queue1, process *queue2, process *queue3, process *queue4)
{

}
void main()
{
    char fileName[100];
    boolean go = false;
    int cpus = 0, sliceDuration = 0, totalMemory = 0;
    process *queue0, *queue1, *queue2, *queue3, *queue4;
    queue0 = createQueue();
    queue1 = createQueue();
    queue2 = createQueue();
    queue3 = createQueue();
    queue4 = createQueue();

    scanf("%d %d %d %s", &cpus, &sliceDuration, &totalMemory, fileName);
    go = readProcessesFile(fileName, queue0, queue1, queue2, queue3, queue4);
    if (go)
        run(cpus, sliceDuration, totalMemory, queue0, queue1, queue2, queue3, queue4);
    printQueue(queue1);
    dequeue(queue1);
    printQueue(queue1);
    printf("End of simulation.\n");

    free(queue0);
    free(queue1);
    free(queue2);
    free(queue3);
    free(queue4);
}
