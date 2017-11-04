#include "paging.h"

void evict_R(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    int max = it->count;
    int r = rand() % PAGE_LIST_SIZE;
    while(it) {
        if(it->pid>0 && count < r){
            page_to_be_evicted = it;
        }
        it = it->next;
        count++;
    }
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
