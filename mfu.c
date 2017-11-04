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
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
