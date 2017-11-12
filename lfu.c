#include "paging.h"

void evict_LFU(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    int min = it->count;
    while(it) {
        if(it->count < min){
            page_to_be_evicted = it;
            min = it->count;
        }
        it = it->next;
    }
    if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", page_to_be_evicted->pid, page_to_be_evicted->count, page_to_be_evicted->last_used);
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
    page_to_be_evicted->state = NOT_IN_MEM;
}
