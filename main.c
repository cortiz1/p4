#include "paging.h"

int main(int argc, char* argv[]) {

    int sim_clock = 0;
    int *page_count_opts = malloc(sizeof(int)*4);
    page_count_opts[0] = 5;
    page_count_opts[1] = 11;
    page_count_opts[2] = 17;
    page_count_opts[3] = 31;

    page_list pl;
    init_page_list(&pl);

    page *tp;

    void (*evict_func)(page_list*);
    if (argc != 2){
        printf("valid options are: fcfs, lru, lfu, mfu or r.\n");
        return -1;
    }
    if(strcmp(argv[1], "fcfs") == 0){
        evict_func = evict_FCFS;
    }else if(strcmp(argv[1], "lru") == 0){
        evict_func = evict_LRU;
    }else if(strcmp(argv[1], "lfu") == 0){
        evict_func = evict_LFU;
    }else if(strcmp(argv[1], "mfu") == 0){
        evict_func = evict_MFU;
    }else if(strcmp(argv[1], "r") == 0){
        evict_func = evict_R;
    }else {
        printf("valid options are: fcfs, lru, lfu, mfu or r.\n");
        return -1;
    }

    srand(0);
    process Q[NUMBER_OF_PROCS];
    for(int i=0;i<NUMBER_OF_PROCS;i++) {
        Q[i].pid = i;
        Q[i].page_count = page_count_opts[rand()%4];
        Q[i].arrival_time = rand()%60;
        Q[i].duration = rand() % MAX_PROC_DURATION;
        Q[i].curr_page = 0; // all processes begin with page 0
    }
    qsort(Q,NUMBER_OF_PROCS,sizeof(process),compare_arrival_times);

    int ix = 0; // index to the start of process queue
    for(sim_clock = 0; sim_clock < SIMULATION_DURATION; sim_clock++) {
//        printf("\n");
//        display_page_list(&pl);
//        printf("\n");
        // at the beginning of every second, look for new processes
        while(ix < NUMBER_OF_PROCS && Q[ix].arrival_time <= sim_clock) {
            // see if we have atleast 4 free pages
            if(test_free_pages(&pl,4)) { // if yes, bring process in memory
                page* p = get_free_page(&pl);
                p->pid = Q[ix].pid;
                p->page_no = Q[ix].curr_page;
                p->brought_in_time = 1.0*sim_clock; // a metric need for FCFS eviction policy
                p->count = 1;
                p->last_used = sim_clock;
                printf("Page %d for process %d brought in at %f\n",Q[ix].curr_page,Q[ix].pid,p->brought_in_time);
                ix++;
            } else break; // not enough memory
        }


        // Every 100ms a new request for a page is being made by all processes in memory.
        for(int i=0;i<10;i++) { // 100ms = 1s / 10, therefore 10 iterations
            for(int j=0;j<ix;j++) if(Q[j].duration > 0) {
                Q[j].curr_page = get_next_page_no(Q[j].curr_page,Q[j].page_count); // update current page no.
                if(page_exists_in_memory(&pl,Q[j].pid,Q[j].curr_page)) { // page already exists in memory
                    // Note that eviction algorithms like LRU, LFU might need to update some metadata at this point. For example, for LRU
                    // we could have another variable 'last_used_time' in the page struct which would be updated to (sim_clock+0.1*i) which is
                    // the time at which this page was referenced again. Since I am implementing FCFS, I don't need any extra book-keeping.
                    tp = get_page_from_pid(&pl,Q[j].pid,Q[j].curr_page);
                    if (tp == NULL){
                        printf("potential bug, got NULL from on a page that exists: pid %d page %d\n", Q[j].pid, Q[j].curr_page);
                        return -1;
                    }

                    tp->count++;
                    tp->last_used = sim_clock;
                    continue;
                }

                // if we are here then that means we refered a page which is not there in memory. So we need to bring it in.

                page* pg = get_free_page(&pl);
                if(!pg) { // no free pages in memory!! need to evict a page
                    printf("Memory full, Page list:\n");
                    display_page_list(&pl);

                    evict_func(&pl);
                    display_page_list(&pl);

                    pg = get_free_page(&pl);
                }
                pg->pid = Q[j].pid;
                pg->page_no = Q[j].curr_page;
                pg->brought_in_time = sim_clock+(0.1*i);
                pg->last_used = sim_clock;
                pg->count = 0;
                printf("Page %d for process %d brought in at %f\n",Q[j].curr_page,Q[j].pid,pg->brought_in_time);
            }

        }

        for(int j=0;j<ix;j++) if(Q[j].duration > 0) {
            Q[j].duration--;
            if(Q[j].duration == 0) { // process has finished execution, free pages in memory
                printf("Process %d done. Freeing memory ... \n",Q[j].pid);
                free_memory(&pl,Q[j].pid);
            }
        }
        usleep(800);
    }
}

