/*
 * FF & RR & CS with their helper functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "scheduling.h"

/* Shortest Remaining Time Scheduling */
void customised_scheduling(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, char* memory_allo){

    int nproc_left = 0;         /* the number of processes that are waiting */
    int nempty_page = npages;   /* the number of empty pages */
    int current_time = 0;       /* current time */


    // while not all processes have been executed
    while(all_plist->head != NULL){

        // check if there is any process that has arrived, if not then just wait
        if(!check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, REM_SORT)){
            current_time++;
        }

        // while there are arrived processes to be executed
        while(arrived_plist->head != NULL){

            // find the process with the shortest remaining time and start the process
            struct node_t* curr = pop_process(arrived_plist);
            start_process(all_plist, arrived_plist, memlist, &curr, &nempty_page, npages, &nproc_left, &current_time, memory_allo, REM_SORT);

            while(1){

                // check if there is any process arriving
                check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, REM_SORT);

                // finish the process when its remaining time reaches 0
                if(curr->data.t_rem == 0){
                    finish_process(finished_plist, memlist, &curr, &nempty_page, npages, &nproc_left, current_time, memory_allo);
                    break;
                }

                // if there is any process with shorter remaining time than the current process, switch to it
                if(arrived_plist->head != NULL && curr->data.t_rem > arrived_plist->head->data.t_rem){
                    push_process(arrived_plist, curr->data);
                    break;
                }

                current_time++;
                curr->data.t_rem--;
            }
        }
    }
    print_statistics(finished_plist, current_time);
}

/* Round Robin Scheduling */
void round_robin(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, int quantum, char* memory_allo){

    int nproc_left = 0;         /* the number of processes that are waiting */
    int nempty_page = npages;   /* the number of available pages */
    int current_time = 0;       /* current time */
    int timer = quantum;        /* a timer with quantum limit */

    while(all_plist->head != NULL){

        /* check if there is any process that has arrived, if not then just wait */
        if(!check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, PID_SORT)){
            current_time++;
        }

        while(arrived_plist->head != NULL && timer >= 0){

            // if two processes arrive at the same time, the one that was not just executed would go first
            struct node_t* curr = pop_process(arrived_plist);
            start_process(all_plist, arrived_plist, memlist, &curr, &nempty_page, npages, &nproc_left, &current_time, memory_allo, PID_SORT);

            while(1){

                // add arrived processes to the queue, awaiting to be executed
                check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, PID_SORT);

                curr->data.t_rem--;
                current_time++;
                timer--;

                // process is finished
                if(curr->data.t_rem == 0){
                    finish_process(finished_plist, memlist, &curr, &nempty_page, npages, &nproc_left, current_time, memory_allo);
                    timer = quantum;
                    break;
                }

                // if quantum time is reached, reset the timer and move onto the next process
                if(timer == 0){

                    // if the 1st reference bit is not empty, record the 2nd reference bit
                    if(strcmp(memory_allo, BYO_MEM) == 0){
                        for(int i = 0; i < npages; i++){
                            if(memlist[i].ref_proc != NULL && memlist[i].ref_proc->pid == curr->data.pid){
                                memlist[i].ref_proc2 = memlist[i].ref_proc;
                                memlist[i].ref_proc2->t_elapsed = current_time;
                            }
                        }
                    }

                    // update the elapsed time of the referenced process
                    if(strcmp(memory_allo, VIRTUAL_MEM) == 0){
                        for(int i = 0; i < npages; i++){
                            if(memlist[i].ref_proc != NULL && memlist[i].ref_proc->pid == curr->data.pid){
                                memlist[i].ref_proc->t_elapsed = current_time;
                            }
                        }
                    }

                    curr->data.t_elapsed = current_time;

                    check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, PID_SORT);

                    push_process(arrived_plist, curr->data);
                    //curr = curr->next;
                    timer = quantum;
                    break;
                }
            }
        }
    }
    print_statistics(finished_plist, current_time);
}

/* check if there is any process arrived at the moment */
int check_incoming_processes(struct queue_t* all_plist, struct queue_t* arrived_plist, int* nproc_left, int current_time, char* sorting_method){
    int incoming = 0;
    /* add to arrived queue if a process just arrived */

    while(all_plist->head != NULL && current_time == all_plist->head->data.arrival_time){

        push_process(arrived_plist, pop_process(all_plist)->data);

        *nproc_left = *nproc_left + 1;
        incoming = 1;
    }

    /* sort in ascending order of their pid */
    if(arrived_plist->head != NULL){

        sort(arrived_plist, sorting_method);

    }

    return incoming;
}

void first_come_first_served(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, char* memory_allo){
    int current_time = 0;
    int nproc_left = 0;         /* the number of processes that are waiting */
    int nempty_page = npages;   /* the number of empty pages */

    while(all_plist->head != NULL){

        /* check if there is any process that has arrived, if not then just wait */
        if(!check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, PID_SORT)){
            current_time++;
        }

        /* runs the arrived processes one by one */
        while(arrived_plist->head != NULL){

            struct node_t* curr = pop_process(arrived_plist);
            /* starts running the top process in the queue */
            start_process(all_plist, arrived_plist, memlist, &curr, &nempty_page, npages, &nproc_left, &current_time, memory_allo, PID_SORT);

            while(curr->data.t_rem >= 0){

                // finish the process if its remaining time reaches 0
                if(curr->data.t_rem == 0){
                    finish_process(finished_plist, memlist, &curr, &nempty_page, npages, &nproc_left, current_time, memory_allo);
                    break;
                }

                /* add arrived processes to the queue, awaiting to be executed */
                check_incoming_processes(all_plist, arrived_plist, &nproc_left, current_time, PID_SORT);

                current_time++;
                curr->data.t_rem--;
            }
        }
    }
    print_statistics(finished_plist, current_time);
}

/* start the current process with given memory management strategy */
void start_process(struct queue_t* all_plist, struct queue_t* arrived_plist, struct page_t* memlist, struct node_t** curr, int* nempty_page, int npages, int* nproc_left, int* current_time, char* memory_allo, char* sorting_method){

    int loading_needed = 1;
    int total_allocated = 0;

    // memory is unlimited
    if(strcmp(memory_allo, UNLIMITED) == 0){
        printf("%d, %s, id=%d, remaining-time=%d\n", *current_time, R, (*curr)->data.pid, (*curr)->data.t_rem);
    }else{

        // apply memory management strategy: swapping-x
        if(strcmp(memory_allo, SWAPPING_X) == 0){
            loading_needed = swapping_x(memlist, (*curr), nempty_page, npages, *current_time, &total_allocated);
        }

        if(strcmp(memory_allo, VIRTUAL_MEM) == 0 || strcmp(memory_allo, BYO_MEM) == 0){

            loading_needed = virtual_memory(memlist, (*curr), nempty_page, npages, *current_time, &total_allocated, memory_allo);
        }

        // apply memory management strategy: virtual memory
        printf("%d, %s, id=%d, remaining-time=%d, load-time=%d, mem-usage=%d%%, mem-addresses=",
               *current_time, R, (*curr)->data.pid, (*curr)->data.t_rem, (*curr)->data.t_load, (int)ceil(nextafter((1.0*(npages - *nempty_page)/npages)*100, 0)));
        print_addresses((*curr)->data.m_addr, (*curr)->data.memsize_req/PSIZE);

        // load pages
        if(loading_needed){
            for(int i = (*curr)->data.t_load; i > 0;i--){

                check_incoming_processes(all_plist, arrived_plist, nproc_left, *current_time, sorting_method);

                (*curr)->data.t_load--;
                (*current_time)++;
            }
        }
    }
}

/* finish the process and print the status info */
void finish_process(struct queue_t* finished_plist, struct page_t* memlist,
                    struct node_t** curr, int* nempty_page, int npages, int* nproc_left, int current_time, char* memory_allo){

    *nproc_left = *nproc_left - 1;
    (*curr)->data.t_finished = current_time; /* for throughput */
    (*curr)->data.t_elapsed = current_time; /* for swapping-x */

    // stores the memory addresses that were evicted
    int* evicted_list = (int*)malloc((int)(*curr)->data.memsize_req/PSIZE * sizeof(int));

    int total_evicted = 0;

    /* if memory management is applied */
    if(strcmp(memory_allo, UNLIMITED) != 0){

        evict(&(*curr)->data, memlist, nempty_page, npages, (*curr)->data.memsize_req/PSIZE, &evicted_list, &total_evicted, memory_allo, F);
        printf("%d, %s, mem-addresses=", current_time, E);
        print_addresses(evicted_list, total_evicted);
    }

    printf("%d, %s, id=%d, proc-remaining=%d\n", current_time, F, (*curr)->data.pid, *nproc_left);

    push_process(finished_plist, (*curr)->data);

    //free(evicted_list);
}