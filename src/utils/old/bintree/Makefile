OBJS := $(patsubst %c,%o, $(wildcard *.c));
PROGS = test
CFLAGS += -pipe -Wall -g 

all: $(OBJS) 

*.o:*.c bintree.h

test: $(OBJS) 
	$(CC) $(CFLAGS) -o test $(OBJS)

clean: 
	rm -rf $(PROGS) $(OBJS) 2>/dev/null || echo

