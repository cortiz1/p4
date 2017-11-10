#include "paging.h"

void init_page_list(page_list* pl) {
    pl->head = malloc(sizeof(page));
    page* it = pl->head;
    for(int i=0;i<PAGE_LIST_SIZE;i++) {
        it->pid = -1; it->page_no = -1;
        it->next = NULL;
        if(i < 99) {
            it->next = malloc(sizeof(page));
            it = it->next;
        }
    }
}

void display_page_list(page_list* pl) {
    page* it = pl->head;
    int cnt = 0;
    while(it) {
        printf(it->pid >= 0 ? "| p[%03d] c:%02d l:%02f |" : "|                  |",it->pid, it->count, it->last_used);
        cnt++;
        if((cnt % 6) == 0) printf("\n");
        it = it->next;
    }
    printf("\n");
}

int test_free_pages(page_list* pl,int count) { // test if atleast {count} no. of pages are free
    page* it = pl->head;
    while(it) {
        if(it->pid == -1) { // page not being used by any process;
            count--;
        }
        if(!count) return 1;
        it = it->next;
    }
    return 0;
}

int page_exists_in_memory(page_list* pl,int pid,int page_no) {
    page* it = pl->head;
    while(it) {
        if(it->pid == pid && it->page_no == page_no) return 1;
        it = it->next;
    }
    return 0;
}

page* get_free_page(page_list* pl) {
    page* it = pl->head;
    while(it) {
        if(it->pid == -1) return it;
        it = it->next;
    }
    return NULL;
}

int free_memory(page_list* pl,int pid) {
    page* it = pl->head;
    int count = 0;
    while(it) {
        if(it->pid == pid) {
            it->pid = -1;
            it->page_no = -1;
            count++;
        }
        it = it->next;
    }
    return count;
}

int get_next_page_no(int curr_page_no,int max_page_size) {
    int x = rand()%10;
    if(x < 7) {
        x = curr_page_no+(rand()%3)-1 < 0;
    } else {
        x = rand()%max_page_size;
        while(abs(x-curr_page_no) <= 1) x = rand()%max_page_size;
    }
    if(x < 0) x = 0;
    if(x >= 100) x = max_page_size-1;
    return x;
}

page* get_page_from_pid(page_list* pl,int pid,int page_no) {
    page* it = pl->head;
    while(it) {
        if(it->pid == pid && it->page_no == page_no) return it;
        it = it->next;
    }
    return NULL;
}

int compare_arrival_times(const void* a,const void* b) {
    return ((process*)a)->arrival_time - ((process*)b)->arrival_time;
}

