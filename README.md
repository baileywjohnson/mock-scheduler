# mock-scheduler
This is a mock CPU scheduling program that implements several different scheduling algorithms (first come first serve, round robin, shortest time remaining first, preemptive priority with aging) given a process trace (does not include I/O, system calls or memory-related interrupts).

Usage: `scheduler [FCFS | RR | SRTF] [PPA] [optional quantum]/[optional increaser]`

The scheduler will prompt for a list of processes defiend by submission time, id and CPU time of each process. These are read in from stdin and the list is terminated by using `0 0 0`. The intended maximum number of processes is 25.
