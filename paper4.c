#include <stdio.h>

int maxBurstTime = 0; //max of all processes
int quantumTime = 0; //0.8 of maxBurstTime
int numberOfProcesses = 0; //number of processes in ready queue
int numberOfProcessesFixed = 0;
int remainingProcesses = 0; //number of remaining processes

int totalWaitTime = 0;
float avgWaitTime = 0;
float avgTurnTime = 0;

int runWithArrivalTime = 0;
int currentArrivalTime = 0;

struct P4Process {
    int arrivalTime; //arrival time
    int burstTime; //burst time
    int processId; //process id
    struct P4Process *next; //next process in queue
    struct P4Process *prev; //prev process in queue, head does not have prev
};

struct P4Process *head = NULL;
struct P4Process *tail = NULL;

int main() {
    //number of processes
    printf("Enter number of processes: ");
    scanf("%d", &numberOfProcesses);
    numberOfProcessesFixed = numberOfProcesses; //for avg

    //initialize the processes with their burst time
    for (int i = 1; i < numberOfProcesses+1; i++) {
        printf("Enter Process %d burst time: ", i);
        
        int burstTime = 0;
        while(scanf("%d", &burstTime) != 1) {
            printf("Process time must be a int!\n");
            printf("Enter Process %d burst time: ", i);
            while (getchar() != '\n') {} //clear scan
        }

        printf("Enter Process %d arrival time: ", i);
        
        int arrivalTime = 0;
        while(scanf("%d", &arrivalTime) != 1) {
            printf("Arrival time must be a int!\n");
            printf("Enter Process %d arrival time: ", i);
            while (getchar() != '\n') {} //clear scan
        }
        
        //set current max burst time
        if (maxBurstTime <= burstTime) {
            maxBurstTime = burstTime;
        }

        runWithArrivalTime += arrivalTime;

        //create new process linked list
        struct P4Process *newProcess = malloc(sizeof(struct P4Process));
        newProcess->burstTime = burstTime;
        newProcess->arrivalTime = arrivalTime;
        newProcess->next = NULL;
        newProcess->prev = NULL;
        newProcess->processId = i;

        if (head == NULL) {
            head = newProcess;
            tail = newProcess;
        }
        else {
            struct P4Process *oldTail = tail;
            tail->next = newProcess;
            tail = newProcess;
            tail->prev = oldTail;
        }
    } 

    if (runWithArrivalTime > 0) { //get the first arrival
        maxBurstTime = head->burstTime;
        arrival();
    }
    else {
        noArrival();
    }
    
    scanf("%d", &numberOfProcesses);
    return 0;
}

int noArrival() {
    printf("Running without arrival time time\n");
    
    //calculate initial quantum time
    quantumTime = maxBurstTime * 0.8;

    //run processes, push back if the process is bigger than quantum time
    printf("Quantum time: %d\n", quantumTime);

    int i = 0;
    while (i <= numberOfProcesses && head != NULL) {
        if (i < numberOfProcesses) {
            //check if process can run within quantum time
            if (head->burstTime <= quantumTime) { //run process
                printf("Process %d is running [burst time: %d], [waiting time: %d], [turnaround time: %d]\n", head->processId, head->burstTime, totalWaitTime, totalWaitTime + head->burstTime);
                avgWaitTime += totalWaitTime;
                avgTurnTime += totalWaitTime + head->burstTime;
                totalWaitTime += head->burstTime;
                numberOfProcesses--;
                i = 0;
                if (head->next != NULL) { //dispose the process that has run
                    head->next->prev = NULL;
                    head = head->next;
                }
                else { //all completed
                    head = NULL;
                    tail = NULL;
                    numberOfProcesses = 0;
                }

            }
            else { //move the process to the end
                printf("Reassigning process %d to after %d\n", head->processId, tail->processId);

                if (head->next == tail) {
                    struct P4Process *temp = head;
                    tail->next = head;
                    head = tail;
                    tail = temp;
                    tail->next = NULL;
                }
                else {
                    tail->next = head;
                    tail = head;

                    if (head->next != NULL) {
                        head->next->prev = NULL;
                        head = head->next;
                    }

                    tail->next = NULL;
                }

                remainingProcesses++;     
            }
        }
        //reiterate the loop, set Quantum time to max again
        else if (i == numberOfProcesses && remainingProcesses > 0 && numberOfProcesses > 0) {
            quantumTime = maxBurstTime;
            numberOfProcesses += remainingProcesses;
            i = 0;
            printf("Quantum Time changed to %d\n", quantumTime);
        }
        i++;
    }

    printf("All Processes completed. Average Waiting Time: %.1f, Average Turnaround Time: %.1f\n", avgWaitTime/numberOfProcessesFixed, avgTurnTime/numberOfProcessesFixed);
    return 0;
}

int arrival() {
    printf("Running with arrival time time\n");

    //calculate initial quantum time
    quantumTime = maxBurstTime * 0.8;
    int i = 0;
    printf("Quantum time: %d\n", quantumTime);

    while (i <= numberOfProcesses && head != NULL) { //using time
        if (head->burstTime > maxBurstTime) { //update burst time
            maxBurstTime = head->burstTime;
            quantumTime = maxBurstTime * 0.8;
            printf("Quantum time changed: %d\n", quantumTime);
        }

        //run process per arrival time
        if (head->arrivalTime == currentArrivalTime) {
            if (head->burstTime <= quantumTime || currentArrivalTime == 0) {
                printf("Process %d arrived at %d and is running [burst time: %d], [waiting time: %d], [turnaround time: %d]\n", head->processId, head->arrivalTime, head->burstTime, totalWaitTime-head->arrivalTime, totalWaitTime + head->burstTime-head->arrivalTime);
                avgWaitTime += totalWaitTime-head->arrivalTime;
                avgTurnTime += totalWaitTime + head->burstTime-head->arrivalTime;
                totalWaitTime += head->burstTime;
                numberOfProcesses--;
                i = 0;
                if (head->next != NULL) { //dispose the process that has run
                    head->next->prev = NULL;
                    head = head->next;
                }
                else { //all completed
                    head = NULL;
                    tail = NULL;
                    numberOfProcesses = 0;
                }
            }
            else if (head->burstTime > quantumTime) { //move to the end
                printf("Process %d arrived at %d but is longer than quantum time\n", head->processId, head->arrivalTime);
                printf("Reassigning process %d to after %d\n", head->processId, tail->processId);

                if (head->next == tail) {
                    struct P4Process *temp = head;
                    tail->next = head;
                    head = tail;
                    tail = temp;
                    tail->next = NULL;
                }
                else {
                    tail->next = head;
                    tail = head;


                    if (head->next != NULL) {
                        head->next->prev = NULL;
                        head = head->next;
                    }

                    tail->next = NULL;
                }

                remainingProcesses++;     
            }
            i++;
        }
        else if (i == numberOfProcesses && remainingProcesses > 0 && numberOfProcesses > 0) {
            quantumTime = maxBurstTime;
            numberOfProcesses += remainingProcesses;
            currentArrivalTime = 0;
            i = 0;
            printf("Quantum Time changed to %d\n", quantumTime);
        }

        currentArrivalTime++;
    }
    printf("All Processes completed. Average Waiting Time: %.1f, Average Turnaround Time: %.1f\n", avgWaitTime/numberOfProcessesFixed, avgTurnTime/numberOfProcessesFixed);

    return 0;
}
