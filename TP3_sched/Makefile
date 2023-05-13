
DIRNOYAU = ..
LIBDIR = $(DIRNOYAU)/lib
INCDIR = $(DIRNOYAU)/include

SCHEDULER = scheduler-dynamique.o
#SCHEDULER = scheduler-aleat.o
#SCHEDULER = scheduler-fifo.o
#SCHEDULER =

CC = cc
CFLAGS = -g -W -Wall -I$(INCDIR)
LDFLAGS = -g -L$(LIBDIR)
LDLIBS = -lnoyau

TEST = test_sched0 test_sched1 test_sched2

all : $(TEST)

scheduler-dynamique.o : scheduler-dynamique.c queue.h
scheduler-aleat.o : scheduler-aleat.c queue.h
scheduler-fifo.o : scheduler-fifo.c queue.h
queue.o : queue.c queue.h

test_sched0.o :
test_sched0 : test_sched0.o queue.o $(SCHEDULER)
	$(CC) $(LDFLAGS) -o $@ test_sched0.o queue.o $(SCHEDULER) $(LDLIBS)

test_sched1.o :
test_sched1 : test_sched1.o queue.o $(SCHEDULER)
	$(CC) $(LDFLAGS) -o $@ test_sched1.o queue.o $(SCHEDULER) $(LDLIBS)

test_sched2.o :
test_sched2 : test_sched2.o queue.o $(SCHEDULER)
	$(CC) $(LDFLAGS) -o $@ test_sched2.o queue.o $(SCHEDULER) $(LDLIBS)

clean :
	-rm -f *.o
	-rm -f $(TEST)
