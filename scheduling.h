#ifndef SCHEDULING_H
#define SCHEDULING_H


#define F_COME_F_SERVED "ff"
#define ROUND_ROBIN "rr"
#define BYO_SCHEDULING "cs"

#define R "RUNNING"
#define E "EVICTED"
#define F "FINISHED"

#include "statistics_and_linkedlist.h"
#include "memory_management.h"



void customised_scheduling(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, char* memory_allo);

void round_robin(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, int quantum, char* memory_allo);

void first_come_first_served(struct queue_t* all_plist, struct queue_t* arrived_plist, struct queue_t* finished_plist, struct page_t* memlist, int npages, char* memory_allo);

void start_process(struct queue_t* all_plist, struct queue_t* arrived_plist, struct page_t* memlist, struct node_t** curr, int* nempty_page, int npages, int* nproc_left, int* current_time, char* memory_allo, char* sorting_method);

int check_incoming_processes(struct queue_t* all_plist, struct queue_t* arrived_plist, int* nproc_left, int current_time, char* sorting_method);

void finish_process(struct queue_t* finished_plist, struct page_t* memlist, struct node_t** curr, int* nempty_page, int npages, int* nproc_left, int current_time, char* memory_allo);



#endif
