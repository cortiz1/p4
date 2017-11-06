#include "paging.h"

void evict_MFU(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    int max = it->count;
    while(it) {
        if(it->count > max){
            page_to_be_evicted = it;
            max = it->count;
        }
        it = it->next;
    }
    if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", page_to_be_evicted->pid, page_to_be_evicted->count, page_to_be_evicted->last_used);
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
