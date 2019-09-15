##############################################################################
# Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
# - Operating Systems Principles, Semester 2, 2019. 
#############################################################################

HEADERS=alloc.h memory_chunk.h list.h main.h osp_string.h
SOURCES=alloc.c memory_chunk.c list.c main.c osp_string.c
OBJECTS=alloc.o memory_chunk.o main.o list.o osp_string.o
CC=gcc
CFLAGS=-Wall -pedantic -std=gnu99 
LDFLAGS=

all: allocate

allocate: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

debug:CFLAGS+=-g -DDEBUG
debug:clean all

sanitize:CFLAGS+=-g -fsanitize=address
sanitize:LDFLAGS+=-fsanitize=address
sanitize:clean all

.PHONY:clean
clean:
	rm -f $(OBJECTS) allocate *~
