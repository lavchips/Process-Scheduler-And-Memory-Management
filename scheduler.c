
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "scheduling.h"
#include "statistics_and_linkedlist.h"
#include "memory_management.h"
/*-----------------------------------------*/
#define FILENAME "-f"
#define SCHEDULING_ALGORITHM "-a"
#define MEMORY_ALLOCATION "-m"
#define MEMORY_SIZE "-s"
#define QUANTUM "-q"

#define MAX_PROCESS 100
/*-----------------------------------------*/
int initialise_memlist(struct page_t* memlist, int memsize);
void file_handling(char* filename, struct queue_t *plist);
void error(char* msg);
/*-----------------------------------------*/

int main(int argc, char** argv) {

    char* scheduling_algo;  /* scheduler algorithm */
    char* memory_allo;      /* memory allocation method */
    int memsize;            /* the size of memory */
    int quantum;            /* a limited time interval for each process to execute on a CPU */

    /* a list of all the processes */
    struct queue_t *all_plist = create_process_queue();

    /* a list of processes that have actually arrived */
    struct queue_t *arrived_plist = create_process_queue();

    /* a list of processes that have been completed */
    struct queue_t *finished_plist = create_process_queue();


    /* input arguments handling */
    for(int i = 1; i < argc; i = i + 2){

        if(strcmp(argv[i], FILENAME) == 0){

            // read from the file and get the total number of processes
            file_handling(argv[i + 1], all_plist);

        }else if(strcmp(argv[i], SCHEDULING_ALGORITHM) == 0){
            scheduling_algo = argv[i + 1];
        }else if(strcmp(argv[i], MEMORY_ALLOCATION) == 0){
            memory_allo = argv[i + 1];
        }else if(strcmp(argv[i], MEMORY_SIZE) == 0){
            memsize = atoi(argv[i + 1]);
        }else if(strcmp(argv[i], QUANTUM) == 0){
            quantum = atoi(argv[i + 1]);
        }else{
            error("Unknown arguments");
        }
    }

    struct page_t *memlist = (struct page_t*)malloc(((int)memsize/PSIZE) * sizeof(struct page_t));
    int npages = initialise_memlist(memlist, memsize);

    /* initialise the second reference bit for each page if customized page replacement algorithm is used */
    if(strcmp(memory_allo, BYO_MEM) == 0){
        for(int i = 0; i < memsize/PSIZE; i++){
            memlist[i].ref_proc2 = NULL;
        }
    }

    /* Scheduling Algorithm Handling */
    if(strcmp(scheduling_algo, F_COME_F_SERVED) == 0){

        first_come_first_served(all_plist, arrived_plist, finished_plist, memlist, npages, memory_allo);

    }else if(strcmp(scheduling_algo, ROUND_ROBIN) == 0){

        round_robin(all_plist, arrived_plist, finished_plist,memlist, npages, quantum, memory_allo);

    }else if(strcmp(scheduling_algo, BYO_SCHEDULING) == 0){

        customised_scheduling(all_plist, arrived_plist, finished_plist, memlist, npages, memory_allo);
    }else{
        error("Unknown Scheduling Algorithm");
    }

    /* free plist memory */
    free_process_queue(all_plist);
    free_process_queue(arrived_plist);
    free_process_queue(finished_plist);
    free(memlist);

    return 0;
}


/* initialise memory list */
int initialise_memlist(struct page_t* memlist, int memsize){

    for(int i=0; i < memsize/PSIZE; i++){
        memlist[i].id = i;
        memlist[i].ref_proc = NULL;
    }
    return memsize/PSIZE;
}

/* Handling the given text file */
void file_handling(char* filename, struct queue_t *plist){

    FILE *fp;

    fp = fopen(filename, "r");
    if(fp == NULL){
        error("Could not open this file");
    }

    struct process_t tmp;

    while(fscanf(fp, "%d %d %d %d", &tmp.arrival_time, &tmp.pid, &tmp.memsize_req, &tmp.job_time) == 4){
        tmp.t_rem = tmp.job_time;
        tmp.t_elapsed = -1;
        tmp.t_load = 0;


        push_process(plist, tmp);
    }
    fclose(fp);
}

/* Print error message and quit the program */
void error(char* msg){
    printf("Error: %s\n", msg);
    exit(0);
}
