for i in {lru,mfu,r,lfu,fcfs};
do
    ./p4 $i > $i.op;
done;

