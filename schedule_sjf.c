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
int wait_time = 0;
int time_elapsed = 0;

// Function Prototypes
Task* pickNextTask();
void print_times();

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

    // Get the next task.
    while((currentTask = pickNextTask()) != NULL) {
        // Note the current time as wait time.
        wait_time = wait_time + time_elapsed;
        run(currentTask, currentTask->burst);
        time_elapsed = currentTask->burst + time_elapsed;

        // Delete the task, since it completed.
        delete(&head, currentTask);
    }

    print_times();
}

/**
 * @brief Function to return the task with shortest CPU burst
 * 
 * @return Task* the task with shortest burst
 */
Task* pickNextTask() {
    if(head == NULL) return NULL;

    struct node *current = head;
    // Note head node's task as shortest initially.
    Task *shrt_task = head->task;
    
    // Interate, and change current, if a shorter job is found.
    while(current->next != NULL) {
        current = current->next;
        if(current->task->burst < shrt_task->burst) {
            shrt_task = current->task;
        }
    }

    return shrt_task;
}

/**
 * @brief Function to average wait, turnaround, and response times.
 * 
 */
void print_times() {
    fprintf(stdout, "\n");
    fprintf(stdout, "Average Waiting Time:    %.3f\n", ((double) wait_time) / ((double) number_tasks));
    fprintf(stdout, "Average Turnaround Time: %.3f\n", ((double) time_elapsed + wait_time) / ((double) number_tasks));
    fprintf(stdout, "Average Response Time:   %.3f\n", ((double) wait_time) / ((double) number_tasks));
}