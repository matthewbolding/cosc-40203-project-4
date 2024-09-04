/*
 * Matthew Bolding
 * Operating Systems 40203-065
 * Project 4 - Schedule Simulator
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

// Global Variables
int next_tid = 0;
struct node *head;
Task *t_task;

int number_tasks = 0;
int time_elapsed = 0;
int total_burst = 0;
int *turnaround_time;
int *response_time;

// Function Prototypes
Task* pickNextTask();
void re_add(Task *task);
void print_times();
float calculate_average(int *array);
int *initialize_array();

/**
 * @brief Function to add a new task to the linked list, anchored at head
 * 
 * @param name the name of the task
 * @param priority the task's priority
 * @param burst the task's CPU burst length
 */
void add(char *name, int priority, int burst) {
    t_task = malloc(sizeof(Task));

    // Set the values of the new Task struct.
    t_task->name = name;
    t_task->tid = __sync_fetch_and_add(&next_tid, 1);
    t_task->priority = priority;
    t_task->burst = burst;

    // Insert it.
    insert(&head, t_task);
    
    number_tasks++;
}

/**
 * @brief Function to invoke the scheduling of all tasks in the linked list
 * 
 */
void schedule() {
    Task *currentTask;
    int burst;

    turnaround_time = malloc(sizeof(int) * number_tasks);
    response_time = initialize_array();

    // Get the next task.
    while((currentTask = pickNextTask()) != NULL) {
        // Note the time elapsed, if this is a task's first time executing.
        if(response_time[currentTask->tid] == -1) response_time[currentTask->tid] = time_elapsed;
        
        // Set the burst appropriately,
        burst = currentTask->burst >= QUANTUM ? QUANTUM : currentTask->burst;
        // and run the task for that burst amount.
        run(currentTask, burst);
        total_burst = total_burst + burst;
        time_elapsed = time_elapsed + burst;

        // If the current task expended all its burst,
        if(currentTask->burst <= QUANTUM) {
            // delete it, and note the time.
            turnaround_time[currentTask->tid] = time_elapsed;
            delete(&head, currentTask);
        } else {
            // Otherwise, add it back to the list (with QUANTUM less burst).
            re_add(currentTask);
        }
    }

    print_times();
}

/**
 * @brief Function to return the last task
 * 
 * @return Task* the last task
 */
Task* pickNextTask() {
    if(head == NULL) return NULL;

    struct node *current = head;
    
    // Find the last node.
    while(current->next != NULL) {
        current = current->next;
    }

    return current->task;
}

/**
 * @brief Function to add a Task to the list, with the same properties but QUANTUM less burst
 * 
 * @param task the task to be readded to the list
 */
void re_add(Task *task) {
    t_task = malloc(sizeof(Task));

    t_task->name = task->name;
    t_task->tid = task->tid;
    t_task->priority = task->priority;
    // Subtract by the QUANTUM.
    t_task->burst = task->burst - QUANTUM;

    // Delete the task it's replacing, and insert the new one.
    delete(&head, task);
    insert(&head, t_task);
}

/**
 * @brief Function to average wait, turnaround, and response times.
 * 
 */
void print_times() {
    fprintf(stdout, "\n");
    fprintf(stdout, "Average Waiting Time:    %.3f\n", calculate_average(turnaround_time) - ((double) total_burst / (double) number_tasks));
    fprintf(stdout, "Average Turnaround Time: %.3f\n", calculate_average(turnaround_time));
    fprintf(stdout, "Average Response Time:   %.3f\n", calculate_average(response_time));
}

/**
 * @brief Helper function to calculate the average of an array
 * 
 * @param array the array
 * @return float the average of the array
 */
float calculate_average(int *array) {
    int sum = 0;
    for(int i = 0; i < number_tasks; i++) {
        sum = array[i] + sum;
    }
    return ((double) sum) / ((double) number_tasks);
}

/**
 * @brief Helper function to initialize a new array of all -1's
 * 
 * @return int* pointer to array
 */
int *initialize_array() {
    int *address = malloc(sizeof(int) * number_tasks);
    for(int i = 0; i < number_tasks; i++) {
        address[i] = -1;
    }
    return address;
}