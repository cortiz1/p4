#include "paging.h"

void evict_FCFS(page_list* pl) {
    page* it = pl->head;
    page* page_to_be_evicted = pl->head;
    while(it) {
        if(it->brought_in_time < page_to_be_evicted->brought_in_time) {
            page_to_be_evicted = it;
        }
        it = it->next;
    }
    page_to_be_evicted->pid = -1;
    page_to_be_evicted->page_no = -1;
}
