CC = gcc
CFLAGS = -O3 -ansi -pedantic -Wall -Wshadow -Wcast-align -Wstrict-prototypes
PROGS =
OBJS = rel.o

ifdef DEBUG
PROGS += test
endif

define EXEC
  $(1)

endef

all : $(OBJS) $(PROGS)

ifdef DEBUG
test : test.c
	$(CC) $(CFLAGS) -o $(@) $(OBJS) test.c
endif

rel.o : rel.h rel.c
	$(CC) $(CFLAGS) -o $(@) -c rel.c

clean :
	$(RM) $(OBJS)

distclean : clean
	$(foreach i,$(PROGS),$(call EXEC,$(RM) $(i)))
