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
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
