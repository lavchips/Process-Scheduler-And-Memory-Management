#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "memory_management.h"

/* Memory Management Method: Virtual Memory */
int virtual_memory(struct page_t* memlist, struct node_t* curr, int* nempty_page, int npages, int current_time, int* total_allocated, char* memory_allo){


    // count the number of pages that have been allocated to this process
    int nallocated = 0;

    // a flag checking when to start evicting
    int start_evict = 0;

    // count the total number of evicted pages
    int total_evicted = 0;

    // count the number of page faults
    int npage_fault = 0;

    // stores the memory addresses that were evicted
    int* evicted_list = (int*)malloc((int)curr->data.memsize_req/PSIZE * sizeof(int));

    // allocate memory if no pages have been allocated to this process
    preallocated_handling(memlist, &curr, &nallocated, npages);

    for(int i = 0; i < curr->data.memsize_req/PSIZE; i++){
        if(nallocated && curr->data.m_addr[i] != -1){

            *total_allocated = *total_allocated + 1;

            continue;
        }
    }


    // iterate through process memory slots that need memory
    for(int i = 0; i < curr->data.memsize_req/PSIZE;i++){

        // count the pages that have been allocated to this process

        if(nallocated && curr->data.m_addr[i] != -1){

           // *total_allocated = *total_allocated + 1;

            continue;
        }

        // if there is not enough memory, starts swapping
        if(*nempty_page == 0 || *nempty_page < curr->data.memsize_req/PSIZE - i - 1 - nallocated){

            // allocate what is left in the physical memory first
            if(*nempty_page > 0 && !start_evict){
                // do nothing
            }else{

                start_evict = 1;

                // sort to find the least-recently-executed process
                struct process_t* tmp_process = NULL;

                // find the 1st lru bit <- v / find lru-2 bit <- cm
                if(strcmp(memory_allo, VIRTUAL_MEM) == 0){
                    find_least_recent_executed_process(1, memlist, curr, npages, &tmp_process);

                }else if(strcmp(memory_allo, BYO_MEM) == 0){

                    find_least_recent_executed_process(2, memlist, curr, npages, &tmp_process);

                    if(tmp_process == NULL){
                        find_least_recent_executed_process(1, memlist, curr, npages, &tmp_process);

                    }
                }

                // if this process has less than 16 kb memory and the available pages are not enough
                if(*total_allocated * PSIZE < MIN_MEM) {
                    // evict one page at a time from the least-recently-executed process
                    evict(tmp_process, memlist, nempty_page, npages, 1, &evicted_list, &total_evicted, memory_allo, E);


                }else{
                    // count page fault for accessing virtual memory
                    npage_fault++;
                    curr->data.m_addr[i] = -1;
                }
            }
        }

        // iterate through all the pages
        for(int j = 0; j < npages; j++){

            /* if the process needs non-virtual memory allocation and there is empty space*/
            if(memlist[j].ref_proc == NULL && npage_fault == 0){
                allocate_physical_memory(memlist, curr, i, j, total_allocated, nempty_page);
                break;
            }
        }
    }
    // add page fault penalty to the remaining execution time of the process
    curr->data.t_rem += npage_fault * PAGE_FAULT_TIME;

    // print evicted addresses if there is any
    print_evicted(evicted_list, total_evicted, current_time);

    return 1;
}


/* sort to find the least-recently-executed process */
void find_least_recent_executed_process(int k, struct page_t* memlist, struct node_t* curr, int npages, struct process_t** tmp_process){

    /* a temporary list to store the least recently executed process found */
    struct queue_t* tmp_plist = create_process_queue();

    /* the elapse time of lru/lur-2 process*/
    int minElapse, minElapse2;

    /* the number of the page that stores lru/lru-2 process*/
    int minIndex, minIndex2;

    // find the page number that stores the lru-k(1 or 2) process
    int isFirst = 1, isFirst2 = 1;
    for(int i = 0; i < npages; i++){

        if (k==1 && memlist[i].ref_proc != NULL) {
            find_lru_id(&memlist[i].ref_proc, i, curr, &isFirst, &minElapse, &minIndex);
        }

        if(k == 2 && memlist[i].ref_proc2 != NULL){

            find_lru_id(&memlist[i].ref_proc2, i, curr, &isFirst2, &minElapse2, &minIndex2);
        }
    }

    if(k == 1){
        push_process(tmp_plist, *memlist[minIndex].ref_proc);
    }

    // making sure minIndex2 is found first before pushing
    if(k == 2 && !isFirst2){
        push_process(tmp_plist, *memlist[minIndex2].ref_proc2);
    }

    // pop the least-recently executed process
    if(tmp_plist->head!= NULL){

        *tmp_process = &pop_process(tmp_plist)->data;

        // reset the second reference bit
        for(int i = 0; i < npages; i++) {
            if (k == 2) {
                if (memlist[i].ref_proc2 != NULL && (*tmp_process)->pid == memlist[i].ref_proc2->pid) {
                    memlist[i].ref_proc2 = NULL;
                    break;
                }
            }
        }

    }

    free_process_queue(tmp_plist);
}

/* Find the number of the page which stores the least-recently-used process*/
void find_lru_id(struct process_t** ref_proc, int i, struct node_t* curr, int *isFirst, int* minElapse, int* minIndex){

    int hasDup = 0;
    for(int j = 0; j < curr->data.memsize_req/PSIZE; j++){
        // if has been allocated to this process
        if(curr->data.pid == (*ref_proc)->pid){
            hasDup = 1;
            break;
        }
    }
    if(!hasDup){

        if((*ref_proc)->t_elapsed > 0){

            if(*isFirst){

                *minElapse = (*ref_proc)->t_elapsed;
                *minIndex = i;
                *isFirst = 0;
            }else{

                if((*ref_proc)->t_elapsed < *minElapse){

                    *minElapse = (*ref_proc)->t_elapsed;
                    *minIndex = i;
                }
            }
        }
    }
}

/* evict a certain number of pages allocated to a particular process */
void evict(struct process_t* process, struct page_t* memlist, int* nempty_page, int npages, int nevict, int** evicted_list, int* total_evicted, char* memory_allo, char* mode){

    int evicted_size = EVICTED_SIZE;

    for(int j = 0; j < npages && nevict > 0; j++){

        // if the page that needs to be evicted is found
        if((memlist[j].ref_proc != NULL && process->pid == memlist[j].ref_proc->pid)){

            // evict `nevict` pages from the process
            if(strcmp(memory_allo, BYO_MEM) == 0){
                if(strcmp(mode, F) == 0){
                    memlist[j].ref_proc2 = NULL;
                }
            }

            memlist[j].ref_proc = NULL;

            for(int i = 0; i < process->memsize_req/PSIZE; i++){
                if(process->m_addr[i] == memlist[j].id){
                    ((*process).m_addr[i]) = -1;
                }
            }

            // keep track of evicted pages
            *nempty_page = *nempty_page + 1;
            nevict--;

            // allocate more memory if the memory of evicted_list is not enough
            if(*total_evicted >= evicted_size - 1){
                evicted_size += 100;
                int* tmp_evicted_list = realloc(*evicted_list, evicted_size * sizeof(evicted_list));

                if(tmp_evicted_list != NULL){
                    *evicted_list = tmp_evicted_list;
                }
            }

            (*evicted_list)[*total_evicted] = memlist[j].id;

            *total_evicted = *total_evicted + 1;
        }
    }

}


/* Memory Management Method: Swapping-x */
int swapping_x(struct page_t* memlist, struct node_t* curr, int* nempty_page, int npages, int current_time, int* total_allocated){

    // count the number of pages that have been allocated to this process
    int nallocated = 0;

    // count the total number of evicted pages
    int total_evicted = 0;

    // stores the memory addresses that were evicted
    int* evicted_list = (int*)malloc((int)EVICTED_SIZE * sizeof(int));

    // allocate memory if no pages have been allocated to this process
    preallocated_handling(memlist, &curr, &nallocated, npages);

    // if there is not enough memory, starts swapping

    while(*nempty_page < curr->data.memsize_req/PSIZE - nallocated){

        // sort to find the least-recently-executed process
        struct process_t* tmp_process = NULL;
        find_least_recent_executed_process(1, memlist, curr, npages, &tmp_process);

        // evict all the pages allocated to the least-recently-executed process
        evict(tmp_process, memlist, nempty_page, npages, tmp_process->memsize_req/PSIZE, &evicted_list, &total_evicted, SWAPPING_X, E);
    }

    // iterate through process memory slots that need memory
    for(int i = 0; i < curr->data.memsize_req/PSIZE;i++){

        // count the pages that have been allocated to this process
        if(nallocated && curr->data.m_addr[i] != -1){
            *total_allocated = *total_allocated + 1;
            continue;
        }

        // iterate through all the pages
        for(int j = 0; j < npages; j++){

            /* if there is empty space */
            if(memlist[j].ref_proc == NULL){
                allocate_physical_memory(memlist, curr, i, j, total_allocated, nempty_page);
                break;
            }
        }
    }

    // print evicted addresses if there is any
    print_evicted(evicted_list, total_evicted, current_time);

    return 1;
}

/* check if there is any page that has been allocated to this process */
void preallocated_handling(struct page_t* memlist, struct node_t** curr, int* nallocated, int npages){

    // if some pages have been allocated, count the number of them
    for(int i = 0; i < npages;i++){
        if(memlist[i].ref_proc != NULL && memlist[i].ref_proc->pid == (*curr)->data.pid) {
            *nallocated = *nallocated + 1;
        }
    }

    // if memory has not been allocated, allocate memory
    if(!*nallocated){
        (*curr)->data.m_addr = (int*)malloc((int)((*curr)->data.memsize_req/PSIZE) * sizeof(int));
        for(int i = 0; i < (*curr)->data.memsize_req/PSIZE; i++){
            (*curr)->data.m_addr[i] = -1;
        }
    }
}

/* allocate physical memory to the process */
void allocate_physical_memory(struct page_t* memlist, struct node_t* curr, int p_i, int m_j, int* total_allocated, int* nempty_page){

    // record process data in the memory list

    memlist[m_j].ref_proc = &curr->data;

    // add the page id to process memory allocated
    curr->data.m_addr[p_i] = memlist[m_j].id;

    // add time for loading each page
    curr->data.t_load += LOAD_TIME;

    *total_allocated = *total_allocated + 1;
    *nempty_page = *nempty_page - 1;
}

/* print evicted addresses if there is any */
void print_evicted(int* evicted_list, int total_evicted, int current_time){
    if(total_evicted > 0){
        printf("%d, %s, mem-addresses=", current_time, E);
        print_addresses(evicted_list, total_evicted);
    }
}

/* print a certain number of page addresses that were allocated to the process */
void print_addresses(int* evicted_list, int total_evicted){


    // sort memory addresses

    for(int i = 0; i < total_evicted - 1; i++){
        int min = i;
        for(int j = i + 1; j < total_evicted; j++){
            if(evicted_list[j] < evicted_list[min]){
                min = j;
            }
        }
        // swap
        int temp = evicted_list[min];

        evicted_list[min] = evicted_list[i];
        evicted_list[i] = temp;
    }

    printf("[");

    for(int i = 0; i < total_evicted; i++){
        if(evicted_list[i] >= 0){
            printf("%d", evicted_list[i]);

            if(i == total_evicted - 1){
                printf("]\n");
            }else{
                printf(",");
            }
        }

    }
    /*int i = 0;
    while(total_evicted > 0){

        if(evicted_list[i] >= 0){
            printf("%d", evicted_list[i]);

            total_evicted--;

            if(total_evicted == 0){
                printf("]\n");
            }else{
                printf(",");
            }
        }
        i++;
    }*/
}
