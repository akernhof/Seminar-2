#include <stdio.h> 
#include <stdlib.h> 

#define NUM_PRIORITY_LEVELS 3 // Define the number of priority levels

// Define a structure for a process
typedef struct _process {
    int pid;                // Process ID
    char name;              // Process name
    int size;               // Process size (workload)
    struct _process *next;  // Pointer to the next process in the round-robin queue
} Process_t;

// Define a structure for a queue
typedef struct _queue {
    Process_t *first;       // Pointer to the first process in the round-robin queue
    Process_t *last;        // Pointer to the last process in the round-robin queue
} Queue_t;

// Define a structure for MLFQ
typedef struct {
    Queue_t queues[NUM_PRIORITY_LEVELS]; // Array of queues for different priority levels
} MLFQ_t;

// Function to create a new MLFQ (Modified Queue Function)
MLFQ_t* createMLFQ() {
    MLFQ_t* mlfq = (MLFQ_t*)malloc(sizeof(MLFQ_t));
    for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
        mlfq->queues[i].first = NULL;
        mlfq->queues[i].last = NULL;
    }
    return mlfq;
}

int pidSequence = 0; // Global variable to keep track of process IDs
int timeSlices[NUM_PRIORITY_LEVELS] = {10, 20, 40}; // Time slices for each priority level

// Function to create a new process
Process_t* createProcess(char NewName, int newSize) {
    Process_t* result = (Process_t *)malloc(sizeof(Process_t));
    result->name = NewName;
    result->next = NULL;
    result->pid = ++pidSequence;
    result->size = newSize;
    return result;
}

// Function to add a process to the appropriate queue based on priority level
void addToQueue(MLFQ_t* mlfq, Process_t* process, int priority) {
    if (priority < 0 || priority >= NUM_PRIORITY_LEVELS) {
        printf("Invalid priority level\n");
        return;
    }

    Queue_t* queue = &mlfq->queues[priority];

    if (queue->last == NULL) {
        queue->first = process;
        queue->last = process;
    } else {
        queue->last->next = process;
        queue->last = process;
    }
    process->next = NULL;
}

// Function to remove and return the first process from the highest priority non-empty queue
Process_t* getRemoveFromQueue(MLFQ_t* mlfq) {
    for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
        Queue_t* queue = &mlfq->queues[i];
        if (queue->first != NULL) {
            Process_t* result = queue->first;
            if (queue->first == queue->last) {
                queue->first = NULL;
                queue->last = NULL;
            } else {
                queue->first = queue->first->next;
            }
            result->next = NULL;
            return result;
        }
    }
    return NULL; // Return NULL if all queues are empty
}

// Function to perform round-robin scheduling for MLFQ
int rr(MLFQ_t* mlfq) {
    while (1) {
        int allQueuesEmpty = 1;
        for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
            Queue_t* queue = &mlfq->queues[i];
            while (queue->first != NULL) {
                allQueuesEmpty = 0;
                Process_t* runningProcess = getRemoveFromQueue(mlfq); // Remove the first process from the highest priority non-empty queue
                int timeSlice = timeSlices[i]; // Get the time slice for the current priority level
                // Print the running process details
                printf(" Process %c with pid = %d is running at level %d with a time slice of %dms; size start = %d; size left = %d \n", 
                        runningProcess->name, runningProcess->pid, i, timeSlice, runningProcess->size, runningProcess->size - timeSlice);
                
                runningProcess->size -= timeSlice; // Decrease the process size by the time slice, simulating execution

                if (runningProcess->size > 0) { // Check if the process still has work left after the time slice
                    int newPriority = (i + 1 < NUM_PRIORITY_LEVELS) ? i + 1 : i; // Move to a lower priority queue if possible
                    printf("\033[31m Process %c with pid = %d is demoted to level %d\033[0m\n", runningProcess->name, runningProcess->pid, newPriority);
                    addToQueue(mlfq, runningProcess, newPriority); // Add the process back to the appropriate queue
                } else {
                    printf("\033[32m ########### Process %c is finished\033[0m\n", runningProcess->name); // Print process finished message
                    free(runningProcess); // Free the memory allocated for the process as it is finished
                }
            }
        }
        if (allQueuesEmpty) {
            break; // Exit the loop if all queues are empty
        }
    }
    return 0; // Return success code
}

// Main function
int main() {
    MLFQ_t* mlfq = createMLFQ(); // Create a new MLFQ

    // Add processes to the highest priority queue (Level 0)
    addToQueue(mlfq, createProcess('A', 10), 0); // Process A with size 10
    addToQueue(mlfq, createProcess('B', 30), 0); // Process B with size 30
    addToQueue(mlfq, createProcess('C', 50), 0); // Process C with size 50

    // Add processes to the second highest priority queue (Level 1)
    addToQueue(mlfq, createProcess('D', 5), 1);  // Process D with size 5
    addToQueue(mlfq, createProcess('E', 40), 1); // Process E with size 40
    addToQueue(mlfq, createProcess('F', 30), 1); // Process F with size 30

    // Add processes to the third highest priority queue (Level 2)
    addToQueue(mlfq, createProcess('G', 20), 2); // Process G with size 20
    addToQueue(mlfq, createProcess('H', 10), 2); // Process H with size 10
    addToQueue(mlfq, createProcess('I', 40), 2); // Process I with size 40
    
    rr(mlfq); // Perform round-robin scheduling on the MLFQ

    return 0;
}
