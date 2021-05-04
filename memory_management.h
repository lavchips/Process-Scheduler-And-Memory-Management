#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#define UNLIMITED "u"
#define SWAPPING_X "p"
#define VIRTUAL_MEM "v"
#define BYO_MEM "cm"

#define PSIZE 4             /* the memory size of each page */
#define LOAD_TIME 2         /* loading time needed */
#define PAGE_FAULT_TIME 1   /* page fault penalty time */
#define MIN_MEM 16          /* the minimum memory requirement of physical memory to execute */
#define EVICTED_SIZE 100    /* the number of evicted page */

#include "statistics_and_linkedlist.h"
#include "scheduling.h"

int virtual_memory(struct page_t* memlist, struct node_t* curr, int* nempty_page, int npages, int current_time, int* total_allocated, char* memory_allo);

int swapping_x(struct page_t* memlist, struct node_t* curr, int* nempty_page, int npages, int current_time, int* total_allocated);

void preallocated_handling(struct page_t* memlist, struct node_t** curr, int* nallocated, int npages);

void find_least_recent_executed_process(int k, struct page_t* memlist, struct node_t* curr, int npages, struct process_t** tmp_process);

void find_lru_id(struct process_t** ref_proc, int i, struct node_t* curr, int *isFirst, int* minElapse, int* minIndex);

void evict(struct process_t* process, struct page_t* memlist, int* nempty_page, int npages, int nevict, int** evicted_list, int* total_evicted, char* memory_allo, char* mode);

void allocate_physical_memory(struct page_t* memlist, struct node_t* curr, int p_i, int m_j, int* total_allocated, int* nempty_page);

void print_evicted(int* evicted_list, int total_evicted, int current_time);

void print_addresses(int* evicted_list, int total_evicted);

#endif
