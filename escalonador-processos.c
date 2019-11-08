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
    int arrival, duration, memory, priority, current, delay, originalPriority, launch, signal;
    struct node *next;
} process;

process *createQueue()
{
    process *head = malloc(sizeof(process));
    head->arrival = 0;
    head->duration = 0;
    head->memory = 0;
    head->priority = 0;
    head->launch = 0;
    head->delay = 0;
    head->originalPriority = 0;
    head->current = 0;
    head->next = NULL;
    head->signal = 0;
    return head;
}

void enqueueProcess(process *p, process *queue)
{
    p->next = queue->next;
    queue->next = p;
}

boolean readProcessesFile(char fileName[], process *queue0, process *queue1, process *queue2, process *queue3, process *queue4)
{
    FILE *f;
    int parametersArray[4];
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
            int i = 0;
            token = strtok(result, ", ");
            while (token != NULL)
            {
                parametersArray[i++] = atoi(token);
                token = strtok(NULL, ", ");
            }
            process *p = malloc(sizeof(process));
            p->arrival = parametersArray[0];
            p->current = 0;
            p->delay = 0;
            p->duration = parametersArray[1];
            p->launch = parametersArray[0];
            p->memory = parametersArray[2];
            p->originalPriority = parametersArray[3];
            p->priority = parametersArray[3];
            p->signal = 0;
            switch (parametersArray[3])
            {
            case 0:
                enqueueProcess(p, queue0);
                break;
            case 1:
                enqueueProcess(p, queue1);
                break;
            case 2:
                enqueueProcess(p, queue2);
                break;
            case 3:
                enqueueProcess(p, queue3);
                break;
            case 4:
                enqueueProcess(p, queue4);
                break;
            }
        }
    }
    fclose(f);
    return true;
}
process* dequeue(process *queue){
    process *p = queue->next, *q = queue;
    if(p == NULL) return NULL;
    while(p->next != NULL){
        p = p->next;
        q = q->next;
    }
    q->next = NULL;
    if(p->originalPriority != 1 && p->originalPriority != 4){
        if(p->signal == 0){
            if(p->priority < 4 && p->priority+1 != p->originalPriority){
                ++p->priority;
            } else {
                p->signal = 1;
                --p->priority;
            }
        } else {
            if(p->priority > 1 && p->priority != p->originalPriority){
                --p->priority;
            } else {
                p->signal = 0;
                ++p->priority;
            }
        }
    }
    return p;
}
void printQueue(process *queue){
    process *p;
    p = queue->next;
    while(p != NULL){
        printf("%d, %d, %d, %d, current: %d\n", p->arrival, p->duration, p->memory, p->priority, p->current);
        p = p->next;
    }
}

void clearCpu(process *cpu[], int i, process *queue0, process *queue1, process *queue2, process *queue3, process *queue4){
    if(cpu[i]->current == cpu[i]->duration){
        FILE *f;
        f = fopen("result.txt", "a");
        if(cpu[i] != NULL){
            fprintf(f, "%d, %d, %d, %d\n", cpu[i]->arrival, cpu[i]->launch, cpu[i]->duration, cpu[i]->delay+cpu[i]->current);
        }
        fclose(f);
    } else{
        switch(cpu[i]->priority){
            case 0:
                enqueueProcess(cpu[i], queue0);
                break;
            case 1:
                enqueueProcess(cpu[i], queue1);
                break;
            case 2:
                enqueueProcess(cpu[i], queue2);
                break;
            case 3:
                enqueueProcess(cpu[i], queue3);
                break;
            case 4:
                enqueueProcess(cpu[i], queue4);
                break;
        }
    }
}

void clearCpus(process *cpu[], int cpus){
    FILE *f;
    int i;
    f = fopen("result.txt", "a");
    for(i=0; i<cpus; i++){
        if(cpu[i] != NULL){
            fprintf(f, "%d, %d, %d, %d\n", cpu[i]->arrival, cpu[i]->launch, cpu[i]->duration, cpu[i]->delay+cpu[i]->current);
        }
    }
    fclose(f);
}

boolean checkCpus(process *cpu[], int cpus){   // checks if cpu processes ended
    int i;
    for(i=0; i<cpus; i++){
        if(cpu[i] != NULL && cpu[i]->duration != cpu[i]->current){
            return false;
        }
    }
    return true;
}

void run(int cpus, int sliceDuration, int totalMemory, process *queue0, process *queue1, process *queue2, process *queue3, process *queue4)
{
    int memorySpace = totalMemory, i = 0, j;
    boolean flag;
    process *p;
    if(cpus > 0 && sliceDuration > 0 && totalMemory >= 64) {
        process *cpu[cpus];
        for(int m = 0; m<cpus; m++){
            cpu[m] = NULL;
        }
        while(cpus){
            printQueue(queue0);
            printQueue(queue1);
            printQueue(queue2);
            printQueue(queue3);
            printQueue(queue4);
            system("pause");
            flag = checkCpus(cpu, cpus);
            p = dequeue(queue0);
            if(p == NULL){
                p = dequeue(queue1);
                if(p == NULL){
                    p = dequeue(queue2);
                    if(p == NULL){
                        p = dequeue(queue3);
                        if(p == NULL){
                            p = dequeue(queue4);
                            if(p == NULL && flag){
                                clearCpus(cpu, cpus);
                                return;
                            };
                        }
                    }
                }
            }
            if(p != NULL && p->memory > totalMemory){
                free(p);
            } else{
                if(p != NULL && memorySpace >= p->memory){
                    for(i = 0; i<cpus; i++){
                        if(cpu[i] == NULL){
                            cpu[i] = p;
                            memorySpace -= cpu[i]->memory;
                            if(cpu[i]->current + sliceDuration < cpu[i]->duration){
                                cpu[i]->current += sliceDuration;
                            } else{
                                cpu[i]->current = cpu[i]->duration;
                                memorySpace += cpu[i]->memory;
                                clearCpu(cpu, i, queue0, queue1, queue2, queue3, queue4);
                                cpu[i] = NULL;
                            }
                            break;
                        } else if(i == cpus-1){
                            if(p->current == 0) ++p->launch;
                            ++p->delay;
                            memorySpace += cpu[0]->memory;
                            clearCpu(cpu, 0, queue0, queue1, queue2, queue3, queue4);
                            cpu[0] = NULL;
                            switch(p->priority){
                                case 1:
                                    enqueueProcess(p, queue1);
                                    break;
                                case 2:
                                    enqueueProcess(p, queue2);
                                    break;
                                case 3:
                                    enqueueProcess(p, queue3);
                                    break;
                                case 4:
                                    enqueueProcess(p, queue4);
                                    break;
                        }
                            break;
                        }
                    }
                } else{
                    clearCpus(cpu, cpus);
                    for(j=0; j<cpus; j++){
                        cpu[j] = NULL;
                    }
                    if(p != NULL)
                    switch(p->priority){
                        case 1:
                            enqueueProcess(p, queue1);
                            break;
                        case 2:
                            enqueueProcess(p, queue2);
                            break;
                        case 3:
                            enqueueProcess(p, queue3);
                            break;
                        case 4:
                            enqueueProcess(p, queue4);
                            break;
                    }
                }
            }
        }
    }

}
void createResultsFile(){
    FILE *f;
    f = fopen("result.txt", "w");
    fclose(f);
}

int main()
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
    if (go){
        createResultsFile();
        run(cpus, sliceDuration, totalMemory, queue0, queue1, queue2, queue3, queue4);
    }
    printQueue(queue1);
    dequeue(queue1);
    printQueue(queue1);
    printf("End of simulation.\n");

    free(queue0);
    free(queue1);
    free(queue2);
    free(queue3);
    free(queue4);

    return 0;
}
