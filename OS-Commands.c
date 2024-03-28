#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "OS-Commands.h"
#include "list.h"

static List* AllPCBS;
static List* ReadyQueue0;
static List* ReadyQueue1;
static List* ReadyQueue2;
static PCB PCBs[LIST_MAX_NUM_NODES];
static SEM SEMs[5];
static int nextFreePCB = 0;
static int nextFreeSEM = 0;
static List* WaitingSend;
static List* WaitingReceive;
static int pidRunning = 0;
static int lastpid;


/*
    Create the initial process
    It is assumed that this process will always be available to run (will
    never block). It must only run if ALL other processes are blocked or if there are no other
    processes. This process can only be killed (exit) if there are no other processes. Once all
    processes (including init) are gone from the system, the simulation must terminate.
*/
void initproc(void){

    lastpid = 0;
    PCB* initProcess = CreatePCB();
    initProcess->pid = lastpid;
    initProcess->state = 1;
    initProcess->priority = 2;
    pidRunning = 0;
    lastpid++;
    AllPCBS = List_create();
    List_append(AllPCBS, initProcess);

    
    ReadyQueue0 = List_create();
    ReadyQueue1 = List_create();
    ReadyQueue2 = List_create();

    WaitingSend = List_create();
    WaitingReceive = List_create();

}


/* 
    Create "C" "int priority (0 = high, 1 = norm, 2 = low)" 
    "create a process and put it on the appropriate ready Q.
    "success or failure, the pid of the created process on success.
*/
int Create(int Priority){

    PCB* newProcess = CreatePCB();
    newProcess->pid = lastpid;
    newProcess->state = 0;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    if(target->pid == 0){
        target->state = 0;
        newProcess->state = 1;
        pidRunning = newProcess->pid;
        newProcess->priority = Priority;
        lastpid++;
        List_append(AllPCBS, newProcess);
        return lastpid - 1;
    }
    newProcess->priority = Priority;
    lastpid++;
    if (List_append(AllPCBS, newProcess)){
        return 0;
    }

    if (Priority == 0){
        if (List_append(ReadyQueue0, newProcess)){
            return 0;
        }
    }else if(Priority == 1){
        if (List_append(ReadyQueue1, newProcess)){
            return 0;
        }
    }else if(Priority == 2){
        if (List_append(ReadyQueue2, newProcess)){
            return 0;
        }
    }else{
        return 0;
    }
    return lastpid - 1;

}


/*
    Fork "F" "None"
    "Copy the currently running process and put it on the ready Q 
    corresponding to the original process' priority. Attempting to Fork 
    the "init" process (see below) should fail.
    "success or failure, the pid of the resulting (new) process on success."
*/
int Fork(void){

    if(pidRunning >= 0){
        PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
        int newPID = Create(target->priority);
        if(newPID == 0){
            return 0;
        }
        return newPID;
    }
    return 0;

}


/*
    Kill "K" "int pid (pid of process to be killed)"
    "kill the named process and remove it from the system."
    "action taken as well as success or failure."
*/
int Kill(int pid){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1 && pid == 0){
        if( List_count(WaitingReceive) == 0 && List_count(WaitingSend) == 0){
            int C1 = 0;
            int C2 = 0;
            for(int i = 0; i < 5; i++){
                if((SEMs + i)->valid == true){
                    C1++;
                    if(List_count((SEMs + i)->semList) == 0){
                        C2++;
                    } 
                }
            }
            if(C1 == C2){
                printf("\n\033[0;31mEnd of program!\033[0m\n\n");
                exit(0);
            }else{
                printf("You can not exit init process when queues are not empty!\n");
                return 0;
            }
        }else{
            printf("You can not exit init process when queues are not empty!\n");
            return 0;
        }
    }
    if(pid != pidRunning){
        PCB* target = List_search(AllPCBS, pComparator, &pid);
        if(target != NULL){
            List_remove(AllPCBS);
        }
        target = List_search(ReadyQueue0, pComparator, &pid);
        if(target != NULL){
            List_remove(ReadyQueue0);
        }
        target = List_search(ReadyQueue1, pComparator, &pid);
        if(target != NULL){
            List_remove(ReadyQueue1);
        }
        target = List_search(ReadyQueue2, pComparator, &pid);
        if(target != NULL){
            List_remove(ReadyQueue2);
        }
        target = List_search(ReadyQueue0, pComparator, &pid);
        if(target != NULL){
            List_remove(ReadyQueue0);
        }
        target = List_search(WaitingSend, pComparator, &pid);
        if(target != NULL){
            List_remove(WaitingSend);
        }
        target = List_search(WaitingReceive, pComparator, &pid);
        if(target != NULL){
            List_remove(WaitingReceive);
        }
    }else{
        Exit();
    }



    // check the sem for delete

    return 1;

}


/*
    Exit "E" "None" "kill the currently running process."
    "process scheduling information (eg. which process now gets
    control of the CPU)"
*/
void Exit(void){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1){
        if( List_count(WaitingReceive) == 0 && List_count(WaitingSend) == 0){
            int C1 = 0;
            int C2 = 0;
            for(int i = 0; i < 5; i++){
                if((SEMs + i)->valid == true){
                    C1++;
                    if(List_count((SEMs + i)->semList) == 0){
                        C2++;
                    } 
                }
            }
            if(C1 == C2){
                printf("\n\033[0;31mEnd of program!\033[0m\n\n");
                exit(0);
            }else{
                printf("You can not exit init process when queues are not empty!\n");
                return;
            }
        }else{
            printf("You can not exit init process when queues are not empty!\n");
            return;
        }
    }
    PCB* temp;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(AllPCBS);
    }
    target = List_search(ReadyQueue0, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(ReadyQueue0);
    }
    target = List_search(ReadyQueue1, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(ReadyQueue1);
    }
    target = List_search(ReadyQueue2, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(ReadyQueue2);
    }
    target = List_search(ReadyQueue0, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(ReadyQueue0);
    }
    target = List_search(WaitingSend, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(WaitingSend);
    }
    target = List_search(WaitingReceive, pComparator, &pidRunning);
    if(target != NULL){
        List_remove(WaitingReceive);
    }


    if(List_count(ReadyQueue0) != 0){
        List_first(ReadyQueue0);
        temp = (PCB*) List_curr(ReadyQueue0);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue0);
    }else if(List_count(ReadyQueue1) != 0){
        List_first(ReadyQueue1);
        temp = (PCB*) List_curr(ReadyQueue1);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue1);
    }else if(List_count(ReadyQueue2) != 0){
        List_first(ReadyQueue2);
        temp = (PCB*) List_curr(ReadyQueue2);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue2);
    }else{
        int initprocess = 0;
        target = List_search(AllPCBS, pComparator, &initprocess);
        target->state = 1;
        pidRunning = initprocess;
    }

}


/*
    Quantum "Q" "None"
    "time quantum of running process expires."
    "action taken (eg. process scheduling information)"
*/
void Quantum(void){

    PCB* temp;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    if(target != NULL){
        target->state = 0;
    }

    if (target->priority == 0){
        List_append(ReadyQueue0, target);
    }else if(target->priority == 1){
        List_append(ReadyQueue1, target);
    }else if(target->priority == 2){
        List_append(ReadyQueue2, target);
    }

    if(List_count(ReadyQueue0) != 0){
        List_first(ReadyQueue0);
        temp = List_curr(ReadyQueue0);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue0);
    }else if(List_count(ReadyQueue1) != 0){
        List_first(ReadyQueue1);
        temp = List_curr(ReadyQueue1);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue1);
    }else if(List_count(ReadyQueue2) != 0){
        List_first(ReadyQueue2);
        temp = List_curr(ReadyQueue2);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue2);
    }else{
        int initprocess = 0;
        target = List_search(AllPCBS, pComparator, &initprocess);
        target->state = 1;
        pidRunning = initprocess;
    }

}


/*
    Send "s" "pid (pid of process to send message to), 
    char *msg (nullterminated message string, 40 char max)"
    "send a message to another process - block until reply"
    "success or failure, scheduling information, and reply
    source and text (once reply arrives)"
*/
int Send(int pid, char* msg){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1){
        terminate->message = msg;
        terminate->pidReceiver = pid;
        PCB* search = List_search(WaitingReceive, pComparator, &pid);
        if(search != NULL){
            search->message = msg;
            search->state = 0;
            List_remove(WaitingReceive);
            if (search->priority == 0){
                List_append(ReadyQueue0, search);
            }else if(search->priority == 1){
                List_append(ReadyQueue1, search);
            }else if(search->priority == 2){
                List_append(ReadyQueue2, search);
            }
        }
        return 1;
    }
    PCB* temp;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    target->state = 2;
    target->message = msg;
    target->pidReceiver = pid;
    if (List_append(WaitingSend, target)){
        return 0;
    }

    if(List_count(ReadyQueue0) != 0){
        List_first(ReadyQueue0);
        temp = List_curr(ReadyQueue0);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue0);
    }else if(List_count(ReadyQueue1) != 0){
        List_first(ReadyQueue1);
        temp = List_curr(ReadyQueue1);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue1);
    }else if(List_count(ReadyQueue2) != 0){
        List_first(ReadyQueue2);
        temp = List_curr(ReadyQueue2);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue2);
    }else{
        int initprocess = 0;
        target = List_search(AllPCBS, pComparator, &initprocess);
        target->state = 1;
        pidRunning = initprocess;
    }

    PCB* search = List_search(WaitingReceive, pComparator, &pid);
    if(search != NULL){
        search->message = msg;
        search->state = 0;
        List_remove(WaitingReceive);
        if (search->priority == 0){
            List_append(ReadyQueue0, search);
        }else if(search->priority == 1){
            List_append(ReadyQueue1, search);
        }else if(search->priority == 2){
            List_append(ReadyQueue2, search);
        }

    }
    return 1;

}


/*
    Receive "R" "None"
    "receive a message - block until one arrives"
    "scheduling information and (once msg is received) the
    message text and source of message"
*/
void Receive(void){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1){
        Node* current = WaitingSend->head;
        while(current != NULL){
            if(init == ((PCB*) current->data)->pidReceiver){
                terminate->message = ((PCB*) current->data)->message;
                terminate->state = 0;
            }
            current = current->next;
        }
        return;
    }
    PCB* temp;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    target->state = 2;
    int lastRuningpid = pidRunning;
    List_append(WaitingReceive, target);

    
    if(List_count(ReadyQueue0) != 0){
        List_first(ReadyQueue0);
        temp = List_curr(ReadyQueue0);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue0);
    }else if(List_count(ReadyQueue1) != 0){
        List_first(ReadyQueue1);
        temp = List_curr(ReadyQueue1);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue1);
    }else if(List_count(ReadyQueue2) != 0){
        List_first(ReadyQueue2);
        temp = List_curr(ReadyQueue2);
        temp->state = 1;
        pidRunning = temp->pid;
        List_remove(ReadyQueue2);
    }else{
        int initprocess = 0;
        target = List_search(AllPCBS, pComparator, &initprocess);
        target->state = 1;
        pidRunning = initprocess;
    }

    Node* current = WaitingSend->head;
    while(current != NULL){
        if(lastRuningpid == ((PCB*) current->data)->pidReceiver){
            target->message = ((PCB*) current->data)->message;
            target->state = 0;
            List_search(WaitingReceive, pComparator, &lastRuningpid);
            List_remove(WaitingReceive);
            if (target->priority == 0){
                List_append(ReadyQueue0, target);
                break;
            }else if(target->priority == 1){
                List_append(ReadyQueue1, target);
                break;
            }else if(target->priority == 2){
                List_append(ReadyQueue2, target);
                break;
            }
        }
        current = current->next;
    }

}


/*
    Reply "Y" "int pid (pid of process to make the reply to),
    char *msg (nullterminated reply string, 40 char max)"
    "unblocks sender and delivers reply"
    "success or failure"
*/
int Reply(int pid, char* msg){


    Node* current = WaitingSend->head;
    while(current != NULL){
        if(pid == ((PCB*) current->data)->pid){
            ((PCB*) current->data)->message = msg;
            ((PCB*) current->data)->state = 0;
            List_search(WaitingSend, pComparator, &pid);
            List_remove(WaitingSend);
            if (((PCB*) current->data)->priority == 0){
                List_append(ReadyQueue0, ((PCB*) current->data));
                return 1;
            }else if(((PCB*) current->data)->priority == 1){
                List_append(ReadyQueue1, ((PCB*) current->data));
                return 1;
            }else if(((PCB*) current->data)->priority == 2){
                List_append(ReadyQueue2, ((PCB*) current->data));
                return 1;
            }
        }
        current = current->next;
    }
    return 0;

}


/*
    New Semaphore "N" "int semaphore (semphore ID), initial value (0 or higher)""
    "Initialize the named semaphore with the value given. ID's can take a value
    from 0 to 4. This can only be done once for a semaphore - subsequent attempts result in error."
    "action taken as well as success or failure."
*/
int NewSemaphore(int semaphoreID, int initial){

    if (nextFreeSEM >= 5) {
        return 0; // No more SEM available
    }
    
    SEM* sem = &SEMs[nextFreeSEM];
    sem->valid = true;
    sem->semid = semaphoreID;
    sem->semaphoreValue = initial;
    sem->semList = List_create();
    nextFreeSEM++;

    return 1;

}


/*
    Sempahore P "P" "int semaphore (semphore ID)"
    "execute the semaphore P operation on behalf of the running process. You can
    assume sempahores IDs numbered 0 through 4."
    "action taken (blocked or not) as well as success or failure."
*/
int SemaphoreP(int semaphoreID){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1){
        printf("You can not use semaphores with initi process!\n");
        return 0;
    }

    PCB* temp;
    PCB* target = List_search(AllPCBS, pComparator, &pidRunning);
    
    for(int i = 0; i < 5; i++){
        if((SEMs + i)->valid == true && (SEMs + i)->semid == semaphoreID){
            (SEMs + i)->semaphoreValue--;
            if((SEMs + i)->semaphoreValue < 0){
                target->state = 2;
                List_append((SEMs + i)->semList, target);

                if(List_count(ReadyQueue0) != 0){
                    List_first(ReadyQueue0);
                    temp = List_curr(ReadyQueue0);
                    temp->state = 1;
                    pidRunning = temp->pid;
                    List_remove(ReadyQueue0);
                }else if(List_count(ReadyQueue1) != 0){
                    List_first(ReadyQueue1);
                    temp = List_curr(ReadyQueue1);
                    temp->state = 1;
                    pidRunning = temp->pid;
                    List_remove(ReadyQueue1);
                }else if(List_count(ReadyQueue2) != 0){
                    List_first(ReadyQueue2);
                    temp = List_curr(ReadyQueue2);
                    temp->state = 1;
                    pidRunning = temp->pid;
                    List_remove(ReadyQueue2);
                }else{
                    int initprocess = 0;
                    target = List_search(AllPCBS, pComparator, &initprocess);
                    target->state = 1;
                    pidRunning = initprocess;
                }
            }
            return 1;
        }
    }
    return 0;

}


/*
    Sempahore V "V" "int semaphore (semphore ID)"
    "execute the semaphore V operation on behalf of the running process. You can
    assume sempahores IDs numbered 0 through 4."
    "action taken (whether/ which process was readied) as well as success or failure."
*/
int SemaphoreV(int semaphoreID){

    int init = 0;
    PCB* terminate = List_search(AllPCBS, pComparator, &init);
    if(terminate->state == 1){
        printf("You can not use semaphores with initi process!\n");
        return 0;
    }

    PCB* temp;
    for(int i = 0; i < 5; i++){
        if((SEMs + i)->valid == true && (SEMs + i)->semid == semaphoreID){
            (SEMs + i)->semaphoreValue++;
            if((SEMs + i)->semaphoreValue >= 0){
                List_last((SEMs + i)->semList);
                temp = List_remove((SEMs + i)->semList);
                temp->state = 0;
                if (temp->priority == 0){
                    List_append(ReadyQueue0, temp);
                }else if(temp->priority == 1){
                    List_append(ReadyQueue1, temp);                   
                }else if(temp->priority == 2){
                    List_append(ReadyQueue2, temp);              
                }
            }
            return 1;
        }
    }
    return 0;

}


/*
    Procinfo "I" "int pid (pid of process information is to be returned for)"
    "dump complete state information of process to screen (this includes process
    status and anything else you can think of)"
    "see Action"
*/
void ProcInfo(int pid){

    PCB* target = List_search(AllPCBS, pComparator, &pid);
    if (target != NULL){
        printf("Information of the process\033[1m %d\033[0m:\n", pid);
        printf("[init process is 0 / state ready = 0, running = 1, blocked = 2 / Priority high = 0, norm = 1, low = 2]\n");
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", target->pid, target->state, target->priority, target->message);
    }else{
        printf("Not Found!\n");
    }

}


/*
    Totalinfo "T" "None" 
    "display all process queues and their contents
    "see Action"
*/
void TotalInfo(void){

    printf("Information of all the processes:\n");
    Node* current = AllPCBS->head;
    printf("[init process is 0 / state ready = 0, running = 1, blocked = 2 / Priority high = 0, norm = 1, low = 2]\n");
    printf("------------------All Processes------------------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    current = ReadyQueue0->head;
    printf("---------All Processes In Ready Queue 0----------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    current = ReadyQueue1->head;
    printf("---------All Processes In Ready Queue 1----------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    current = ReadyQueue2->head;
    printf("---------All Processes In Ready Queue 2----------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    current = WaitingSend->head;
    printf("--------All Processes In Wait Queue Send---------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    current = WaitingReceive->head;
    printf("-------All Processes In Wait Queue Receive-------\n");
    while (current != NULL){
        printf("process ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
        current = current->next;
    }
    printf("------------------All semaphores-----------------\n");
    for(int i = 0; i < 5; i++){
        if((SEMs + i)->valid == true){
            printf("SEM ID:\033[1;31m %d\033[0m Value:\033[0;32m %d\033[0m List of process in this semaphore:\n", ((SEM*)(SEMs + i))->semid, ((SEM*)(SEMs + i))->semaphoreValue);
            current = (SEMs + i)->semList->head;
            while(current != NULL){
                printf("\tprocess ID:\033[1;31m %d\033[0m State:\033[0;32m %d\033[0m Priority:\033[0;36m %d\033[0m process message:\033[1;33m %s\033[0m\n", ((PCB*) current->data)->pid, ((PCB*) current->data)->state, ((PCB*) current->data)->priority, ((PCB*) current->data)->message);
                current = current->next;
            }
        }
        
    }

} 


/*
    compare function for search
*/
bool pComparator(void* pItem, void* pComparisonArg) {
    return (((PCB*) pItem)->pid == *((int*)pComparisonArg));
}


/*
    Create a process and return the pointer
*/
PCB* CreatePCB(){
    if (nextFreePCB >= LIST_MAX_NUM_NODES) {
        return NULL; // No more nodes available
    }
    for (int i = nextFreePCB; i < LIST_MAX_NUM_NODES; i++){
        PCB* PCB = &PCBs[nextFreePCB];
        if (PCB->valid != true){
            PCB->valid = true;
            nextFreePCB++;
            return PCB;
        }
    }

    for (int i = 0; i < nextFreePCB; i++){
        PCB* PCB = &PCBs[nextFreePCB];
        if (PCB->valid != true){
            PCB->valid = true;
            nextFreePCB++;
            return PCB;
        }
    }

    return NULL;
}


// End of OS-Commands.c