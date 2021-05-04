#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "statistics_and_linkedlist.h"


/* print statistics */
void print_statistics(struct queue_t* finished_plist, int current_time){
    print_throughput(finished_plist, current_time);
    print_turnaround(finished_plist);
    print_overhead(finished_plist);
    printf("Makespan %d\n", current_time);
}

/* calculate throughput */
void print_throughput(struct queue_t* plist, int makespan){

    // sort the completed processes by their finishing time
    sort(plist, FINISHED_SORT);

    // records the total throughput
    int total = 0;

    // records the maximum and minimum throughput
    int maximum, minimum;

    // records the throughput for each interval
    int count = 0;

    // the sequence of interval
    int i = 1;

    struct node_t* curr = plist->head;
    while(curr != NULL){
        while(1){

            // if the process finished between this interval
            if(curr->data.t_finished >= i && curr->data.t_finished <= i + INTERVAL){

                count++;

                /* move to the next interval if the next process finished beyond the current interval */
                if(curr->next == NULL || curr->next->data.t_finished > i + INTERVAL){

                    /* record the total, maximum and minimum freq of every interval */
                    total += count;
                    if(i == 1){
                        maximum = minimum = count;

                    }else{
                        if(count > maximum){
                            maximum = count;
                        }
                        if(count < minimum){
                            minimum = count;
                        }
                    }
                }
                break;
            }else{
                // increment the interval by 60 if that is within makespan
                if(i + 1 + INTERVAL <= makespan){

                    if(count == 0){
                        minimum = 0;
                        if(i == 1){
                            maximum = count;
                        }
                    }

                    i = i + 1 + INTERVAL;

                    // reset freq counter
                    count = 0;
                }
            }
        }
        curr = curr->next;
    }

    // average = total / [((last_left_bound - 1)/interval) + 1]
    printf("Throughput %d, %d, %d\n", (int)ceil((double)total/(((i - 1)/INTERVAL) + 1)), minimum, maximum);
}

/* calculate turnaround time */
void print_turnaround(struct queue_t* plist){
    struct node_t* curr = plist->head;
    int total_turnaround = 0;
    int nproc = 0;

    while(curr != NULL){
        total_turnaround += curr->data.t_finished - curr->data.arrival_time;
        nproc++;
        curr = curr->next;
    }
    printf("Turnaround time %d\n", (int)ceil((double)total_turnaround/nproc));
}

/* calculate time overhead */
void print_overhead(struct queue_t* plist){
    struct node_t* curr = plist->head;
    int nproc = 0;
    double total_overhead = 0;
    double maximum;
    while(curr != NULL){
        total_overhead += (curr->data.t_finished - curr->data.arrival_time) / (double)curr->data.job_time;
        if(nproc == 0){
            maximum = (curr->data.t_finished - curr->data.arrival_time) / (double)curr->data.job_time;
        }else{
            if(maximum < (curr->data.t_finished - curr->data.arrival_time) / (double)curr->data.job_time){
                maximum = (curr->data.t_finished - curr->data.arrival_time) / (double)curr->data.job_time;
            }
        }
        nproc++;
        curr = curr->next;
    }
    printf("Time overhead %.2f %.2f\n", maximum, total_overhead/nproc);
}

/* sort the process list by the given method*/
void sort(struct queue_t* plist, char* method){

    struct node_t *curr = plist->head;
    while(curr != NULL){

        struct node_t *tmp = curr;
        struct node_t *right = curr->next;

        while(right != NULL){

            // sort by pid in ascending order
            if(strcmp(method, PID_SORT) == 0){
                if(tmp->data.t_elapsed > 0 && right->data.arrival_time < 0){
                    if(tmp->data.t_elapsed == right->data.arrival_time && tmp->data.pid > right->data.pid){
                        tmp = right;
                    }
                }else if(tmp->data.t_elapsed > 0 && right->data.t_elapsed > 0){
                    if(tmp->data.t_elapsed == right->data.t_elapsed && tmp->data.pid > right->data.pid){
                        tmp = right;
                    }
                }else if(tmp->data.t_elapsed < 0 && right->data.t_elapsed < 0){
                    if(tmp->data.arrival_time == right->data.arrival_time && tmp->data.pid > right->data.pid){
                        tmp = right;
                    }
                }
                // sort by finished time in ascending order
            }else if(strcmp(method, FINISHED_SORT) == 0){
                if(tmp->data.t_finished > right->data.t_finished){
                    tmp = right;
                }
                // sort by elapsed time in ascending order
            }else if(strcmp(method, ELAPSED_SORT) == 0){
                if(tmp->data.t_elapsed > right->data.t_elapsed) {
                    tmp = right;
                }
            }else if(strcmp(method, REM_SORT) == 0){
                if(tmp->data.t_rem > right->data.t_rem || (tmp->data.t_rem == right->data.t_rem && tmp->data.pid > right->data.pid)) {
                    tmp = right;
                }
            }
            right = right->next;
        }

        // swap
        struct process_t tmp_val = curr->data;
        curr->data = tmp->data;
        tmp->data = tmp_val;
        curr = curr->next;
    }
}

/** Basic Linked List setup */
struct node_t* create_node(struct process_t p_info){
    struct node_t* tmp = (struct node_t*)malloc(sizeof(struct node_t));
    tmp->data = p_info;
    tmp->next = NULL;
    return tmp;
}

struct queue_t* create_process_queue(){
    struct queue_t* queue = (struct queue_t*)malloc(sizeof(struct queue_t));
    queue->head = queue->foot = NULL;
    return queue;
};


void push_process(struct queue_t* queue, struct process_t p_info){
    struct node_t* tmp = create_node(p_info);
    if(queue->foot == NULL){
        queue->head = queue->foot = tmp;
        return;
    }
    queue->foot->next = tmp;
    queue->foot = tmp;
}

struct node_t* pop_process(struct queue_t* queue){
    struct node_t* node;
    assert(queue->head != NULL);

    struct node_t* tmp = queue->head;
    node = tmp;
    queue->head = queue->head->next;

    if(queue->head == NULL){
        queue->foot = NULL;
    }
    return node;
}

void free_process_queue(struct queue_t* queue){
    struct node_t *curr, *prev;
    assert(queue != NULL);
    curr = queue->head;
    while(curr){
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(queue);
}

