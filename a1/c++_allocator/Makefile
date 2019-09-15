##############################################################################
# Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
# - Operating Systems Principles, Semester 2, 2019. 
#############################################################################
HEADERS=alloc.h memory_chunk.h memory_list.h main.h
SOURCES=alloc.cpp memory_chunk.cpp main.cpp
OBJECTS=alloc.o memory_chunk.o main.o
CC=g++
CXXFLAGS=-Wall -pedantic -std=c++17 -O3
LDFLAGS=

all: allocate

allocate: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c $<

debug:CXXFLAGS+=-g -DDEBUG
debug:clean all

sanitize:CXXFLAGS+=-g -fsanitize=address
sanitize:LDFLAGS+=-fsanitize=address
sanitize:clean all

.PHONY:clean
clean:
	rm -f $(OBJECTS) allocate *~
