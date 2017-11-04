#ifndef PAGING_H
#define PAGING_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUMBER_OF_PROCS 150
#define SIMULATION_DURATION 60 
#define MAX_PROC_DURATION 7

extern int sim_clock;
extern int *page_count_opts;

typedef struct {
    int pid, page_count, arrival_time, duration, curr_page;
} process;

typedef struct page {
    int pid; // to determine which process currently holds the page, if none does then = -1
    int page_no;
    struct page* next;

    float brought_in_time; // denotes the time the page was first brought in memory
} page;

typedef struct {
    page* head;
} page_list;


void init_page_list(page_list*);
void display_page_list(page_list*);
int test_free_pages(page_list*,int); // test if atleast {count} no. of pages are free
int page_exists_in_memory(page_list*,int,int);
page* get_free_page(page_list*);
void free_memory(page_list*,int);
void evict_FCFS(page_list*);
int get_next_page_no(int,int);
int compare_arrival_times(const void* ,const void*);


#endif