CC = gcc
CFLAGS = -O3 -ansi -pedantic -Wall -Wshadow -Wcast-align -Wstrict-prototypes
PROGS =
OBJS = list.o rel.o network.o

DEBUG=1

ifdef DEBUG
PROGS += test
endif

define EXEC
  $(1)

endef

all : $(OBJS) $(PROGS)

ifdef DEBUG
test : test.c $(OBJS)
	$(CC) $(CFLAGS) -o $(@) $(OBJS) test.c
endif

rel.o : tribe.h rel.h rel.c
	$(CC) $(CFLAGS) -o $(@) -c rel.c

list.o : tribe.h list.h list.c
	$(CC) $(CFLAGS) -o $(@) -c list.c

network.o : tribe.h list.h network.h network.c
	$(CC) $(CFLAGS) -o $(@) -c network.c

clean :
	$(RM) $(OBJS)

distclean : clean
	$(foreach i,$(PROGS),$(call EXEC,$(RM) $(i)))
