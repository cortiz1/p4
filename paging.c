#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUMBER_OF_PROCS 150
#define SIMULATION_DURATION 60 
#define MAX_PROC_DURATION 7

int sim_clock = 0;
const int page_count_opts[] = {5, 11, 17, 31};

typedef struct {
    int pid, page_count, arrival_time, duration, curr_page;
} process;

typedef struct page {
    int pid; // to determine which process currently holds the page, if none does then = -1
    int page_no;
    struct page* next;

    float brought_in_time; // denotes the time the page was first brought in memory
} page;

typedef struct {
    page* head;
} page_list;

void init_page_list(page_list* pl) {
    pl->head = malloc(sizeof(page));
    page* it = pl->head;
    for(int i=0;i<100;i++) {
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
        printf(it->pid > 0 ? "|%d |" : "|  |",it->pid);
        cnt++;
        if((cnt % 20) == 0) printf("\n");
        it = it->next;
    }
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

void free_memory(page_list* pl,int pid) {
    page* it = pl->head;
    while(it) {
        if(it->pid == pid) {
            it->pid = -1;
            it->page_no = -1;
        }
        it = it->next;
    }
}


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





int compare_arrival_times(const void* a,const void* b) {
    return ((process*)a)->arrival_time - ((process*)b)->arrival_time;
}

int main() {

    page_list pl;
    init_page_list(&pl);

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
        printf("\n");
        display_page_list(&pl);
        printf("\n");
        // at the beginning of every second, look for new processes
        while(ix < NUMBER_OF_PROCS && Q[ix].arrival_time <= sim_clock) {
            // see if we have atleast 4 free pages
            if(test_free_pages(&pl,4)) { // if yes, bring process in memory
                page* p = get_free_page(&pl);
                p->pid = Q[ix].pid;
                p->page_no = Q[ix].curr_page;
                p->brought_in_time = 1.0*sim_clock; // a metric need for FCFS eviction policy
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
                    continue;
                }

                // if we are here then that means we refered a page which is not there in memory. So we need to bring it in.

                page* pg = get_free_page(&pl);
                if(!pg) { // no free pages in memory!! need to evict a page
                    printf("Memory full. Evicting a page ... \n");
                    evict_FCFS(&pl);
                    pg = get_free_page(&pl);
                } 
                pg->pid = Q[j].pid;
                pg->page_no = Q[j].curr_page;
                pg->brought_in_time = sim_clock+(0.1*i); 
                printf("Page %d for process %d brought in at %f\n",Q[j].curr_page,Q[j].pid,pg->brought_in_time);
            }
            //usleep(100*1000);
        }

        for(int j=0;j<ix;j++) if(Q[j].duration > 0) {
            Q[j].duration--;
            if(Q[j].duration == 0) { // process has finished execution, free pages in memory
                printf("Process %d done. Freeing memory ... \n",Q[j].pid);
                free_memory(&pl,Q[j].pid);
            }
        }
    }
}

