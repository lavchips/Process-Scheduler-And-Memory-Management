#ifndef STATISTICS_AND_LINKEDLIST_H
#define STATISTICS_AND_LINKEDLIST_H

#define PID_SORT "pid"
#define FINISHED_SORT "t_finished"
#define ELAPSED_SORT "t_elapsed"
#define REM_SORT "t_rem"

#define INTERVAL 59

struct process_t{
    int arrival_time; /* time arrived (second) */
    int pid;          /* process id */
    int memsize_req;  /* memory requirement of the process (KB) */
    int job_time;     /* the time needed to finish the job (second) */
    int t_rem;        /* the remaining time until the process is finished (second)*/
    int t_finished;   /* the time that the process is finished (second)*/
    int t_elapsed;    /* the latest time that the process was elapsed */
    int t_load;       /* the time it takes to load process's pages in memory */
    int *m_addr;      /* the address of pages that were allocated to this process */
};

struct page_t{
    int id;
    struct process_t* ref_proc;
    struct process_t* ref_proc2;
};

struct node_t{

    struct process_t data;
    struct node_t *next;
};

struct queue_t{
    struct node_t *head;
    struct node_t *foot;
};

void print_statistics(struct queue_t* finished_plist, int current_time);

void print_throughput(struct queue_t* plist, int makespan);

void print_turnaround(struct queue_t* plist);

void print_overhead(struct queue_t* plist);

void sort(struct queue_t* plist, char* method);

struct node_t* create_node(struct process_t p_info);

struct queue_t* create_process_queue();

void push_process(struct queue_t* queue, struct process_t p_info);

struct node_t* pop_process(struct queue_t* queue);

void free_process_queue(struct queue_t* queue);







#endif
