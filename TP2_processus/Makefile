
NOYAUDIR = ..

CC = cc
CFLAGS = -g -W -Wall -I$(NOYAUDIR)/include
LDFLAGS = -g -L$(NOYAUDIR)/lib
LDLIBS = -lnoyau

TEST = exo0 exo1 exo2 exo3 exo4

all : $(TEST)

exo4 : exo4.o queue.o
exo4.o : exo4.c queue.h
queue.o : queue.c queue.h

clean :
	rm -f *.o
	rm -f $(TEST)

