#include "paging.h"

void evict_R(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    int count = 0;
    int r = rand() % PAGE_LIST_SIZE;
    while(it) {
        if(it->pid>=0 && count < r){
            page_to_be_evicted = it;
        }
        it = it->next;
        count++;
    }
    if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", page_to_be_evicted->pid, page_to_be_evicted->count, page_to_be_evicted->last_used);
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
    page_to_be_evicted->state = NOT_IN_MEM;
}
