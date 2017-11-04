#include "paging.h"

void evict_LRU(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    int lru = it->last_used;
    while(it) {
        if(it->last_used < lru){
            page_to_be_evicted = it;
            lru = it->last_used;
        }
        it = it->next;
    }
    if (DEBUG==1) printf("EVICTED: p[%03d] c:%02d l:%02d\n", page_to_be_evicted->pid, page_to_be_evicted->count, page_to_be_evicted->last_used);
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
