#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "OS-Commands.h"
#include "list.h"




int main(){

    printf("\n\033[1mWellcome to my OS simulation!\033[0m\n");

    initproc();
    
    char command;
    char msg[40];
    int P1;
    int P2;
    int result;
    
    
    while (1)
    {
        // get the commands
        printf("\nEnter your command character: ");
        scanf("%c", &command);
        
        switch(command){
            case 'C':
            case 'c':
                printf("Create:\n");
                printf("Enter the priority for Create: ");
                scanf("%d", &P1);
                if(Create(P1) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'F':
            case 'f':
                printf("Fork:\n");
                if((result = Fork()) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done! new pid: %d\n",result);
                }
                break;
            
            case 'K':
            case 'k':
                printf("Kill:\n");
                printf("Enter the pid for Kill: ");
                scanf("%d", &P1);
                if(Kill(P1) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'E':
            case 'e':
                printf("Exit:\n");
                Exit();
                printf("Done!\n");
                break;

            case 'Q':
            case 'q':
                printf("Quantum:\n");
                Quantum();
                printf("Done!\n");
                break;

            case 'S':
            case 's':
                printf("Send:\n");
                printf("Enter the pid for Send: ");
                scanf("%d", &P1);
                printf("Enter the message for Send: ");
                scanf("%s", msg);
                if(Send(P1, msg) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'R':
            case 'r':
                printf("Receive:\n");
                Receive();
                printf("Done!\n");
                break;
            
            case 'Y':
            case 'y':
                printf("Reply:\n");
                printf("Enter the pid for Reply: ");
                scanf("%d", &P1);
                printf("Enter the message for Reply: ");
                scanf("%s", msg);
                if(Reply(P1, msg) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'N':
            case 'n':
                printf("new semaphore:\n");
                printf("Enter the semaphoreid for new semaphore: ");
                scanf("%d", &P1);
                printf("Enter the initial for new semaphore: ");
                scanf("%d", &P2);
                if(NewSemaphore(P1, P2) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'P':
            case 'p':
                printf("semaphore P:\n");
                printf("Enter the semaphoreid for semaphore P: ");
                scanf("%d", &P1);
                if(SemaphoreP(P1) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'V':
            case 'v':
                printf("semaphore V:\n");
                printf("Enter the semaphoreid for semaphore V: ");
                scanf("%d", &P1);
                if(SemaphoreV(P1) == 0){
                    printf("Error!\n");
                }else{
                    printf("Done!\n");
                }
                break;

            case 'I':
            case 'i':
                printf("Procinfo:\n");
                printf("Enter the pid for Procinfo: ");
                scanf("%d", &P1);
                ProcInfo(P1);
                printf("Done!\n");
                break;

            case 'T':
            case 't':
                printf("Totalinfo:\n");
                TotalInfo();
                printf("Done!\n");
                break;

            default:
                printf("Invalid Command!\n");
        }

        while ((command = getchar()) != '\n' && command != EOF);

    }

    return 0;
}
// End of OS-main.c