
DIRNOYAU = ..
LIBDIR = $(DIRNOYAU)/lib
INCDIR = $(DIRNOYAU)/include


CC = cc
CFLAGS = -g -W -Wall -I$(INCDIR)
LDFLAGS = -g -L$(LIBDIR)
LDLIBS = -lnoyau

TEST = test_memvirt0 test_memvirt1 test_memvirt2

all : $(TEST)

memvirt.o : memvirt.c

test_memvirt0.o :
test_memvirt0 : test_memvirt0.o memvirt.o
	$(CC) $(LDFLAGS) -o $@ test_memvirt0.o memvirt.o $(LDLIBS)

test_memvirt1.o :
test_memvirt1 : test_memvirt1.o memvirt.o
	$(CC) $(LDFLAGS) -o $@ test_memvirt1.o memvirt.o $(LDLIBS)

test_memvirt2.o :
test_memvirt2 : test_memvirt2.o memvirt.o
	$(CC) $(LDFLAGS) -o $@ test_memvirt2.o memvirt.o $(LDLIBS)

clean :
	-rm -f *.o
	-rm -f $(TEST)
