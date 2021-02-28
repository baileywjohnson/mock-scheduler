PROGS = scheduler
CC = gcc

all: $(PROGS)

scheduler: scheduler.c
	$(CC) -o scheduler scheduler.c

clean:
	rm -f $(PROGS) *.o core*
