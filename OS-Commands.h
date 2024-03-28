#ifndef _OS_COMMANDS_H_
#define _OS_COMMANDS_H_

#define MAX_MSG_LENGHT 40
#define MAX_LIST_NODES 20

#include "list.h"


/*
*   The scheduling algorithm is preemptive round robin with three levels of priority.
*   When a sender is readied, you may want to put something like the 
*    following textin that field: reply received, text is: blah blah blah 
*    This field should be displayed and cleared the next time the process runs.
*/


typedef struct PCB_s{
    int pid;                         // Process ID
    int priority;                    // Priority (0, 1, or 2)
    int state;                       // Process state (e.g., ready = 0, running = 1, blocked = 2)
    bool valid;                      // valid element for create 
    char *message;                   // Place to store message
    int pidReceiver;                 // Place to store the receiver pid 
}PCB;



typedef struct {
    int semid;                       // semaphore ID
    int semaphoreValue;              // Semaphore value 
    bool valid;                      // valid element for create 
    List* semList;                   // List of processes waiting on the semaphore
} SEM;


/*
    Create the initial process
    It is assumed that this process will always be available to run (will
    never block). It must only run if ALL other processes are blocked or if there are no other
    processes. This process can only be killed (exit) if there are no other processes. Once all
    processes (including init) are gone from the system, the simulation must terminate.
*/
void initproc(void);


/* 
    Create "C" "int priority (0 = high, 1 = norm, 2 = low)" 
    "create a process and put it on the appropriate ready Q.
    "success or failure, the pid of the created process on success.
*/
int Create(int Priority);


/*
    Fork "F" "None"
    "Copy the currently running process and put it on the ready Q 
    corresponding to the original process' priority. Attempting to Fork 
    the "init" process (see below) should fail.
    "success or failure, the pid of the resulting (new) process on success."
*/
int Fork(void);


/*
    Kill "K" "int pid (pid of process to be killed)"
    "kill the named process and remove it from the system."
    "action taken as well as success or failure."
*/
int Kill(int pid);


/*
    Exit "E" "None" "kill the currently running process."
    "process scheduling information (eg. which process now gets
    control of the CPU)"
*/
void Exit(void);


/*
    Quantum "Q" "None"
    "time quantum of running process expires."
    "action taken (eg. process scheduling information)"
*/
void Quantum(void);


/*
    Send "s" "pid (pid of process to send message to), 
    char *msg (nullterminated message string, 40 char max)"
    "send a message to another process - block until reply"
    "success or failure, scheduling information, and reply
    source and text (once reply arrives)"
*/
int Send(int pid, char* msg);


/*
    Receive "R" "None"
    "receive a message - block until one arrives"
    "scheduling information and (once msg is received) the
    message text and source of message"
*/
void Receive(void);


/*
    Reply "Y" "int pid (pid of process to make the reply to),
    char *msg (nullterminated reply string, 40 char max)"
    "unblocks sender and delivers reply"
    "success or failure"
*/
int Reply(int pid, char* msg);


/*
    New Semaphore "N" "int semaphore (semphore ID), initial value (0 or higher)""
    "Initialize the named semaphore with the value given. ID's can take a value
    from 0 to 4. This can only be done once for a semaphore - subsequent attempts result in error."
    "action taken as well as success or failure."
*/
int NewSemaphore(int semaphoreID, int initial);


/*
    Sempahore P "P" "int semaphore (semphore ID)"
    "execute the semaphore P operation on behalf of the running process. You can
    assume sempahores IDs numbered 0 through 4."
    "action taken (blocked or not) as well as success or failure."
*/
int SemaphoreP(int semaphoreID);


/*
    Sempahore V "V" "int semaphore (semphore ID)"
    "execute the semaphore V operation on behalf of the running process. You can
    assume sempahores IDs numbered 0 through 4."
    "action taken (whether/ which process was readied) as well as success or failure."
*/
int SemaphoreV(int semaphoreID);


/*
    Procinfo "I" "int pid (pid of process information is to be returned for)"
    "dump complete state information of process to screen (this includes process
    status and anything else you can think of)"
    "see Action"
*/
void ProcInfo(int pid);


/*
    Totalinfo "T" "None" 
    "display all process queues and their contents
    "see Action"
*/
void TotalInfo(void);


/*
    compare function for search
*/
bool pComparator(void* pItem, void* pComparisonArg);


/*
    Create a process and return the pointer
*/
PCB* CreatePCB();


#endif