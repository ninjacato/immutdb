SRCDIR = src
TESTDIR = tests
CC = g++ --std=c++11

OBJECTS = $(SRCDIR)/ImmutDB.cpp $(SRCDIR)/Database.cpp
all: $(OBJECTS)
	$(CC) -o immutdb.exedat $(OBJECTS) -lcaf_core -lrocksdb

DatabaseTests: $(TESTDIR)/DatabaseTests.cpp $(SRCDIR)/Database.cpp
	$(CC) -o $(TESTDIR)/$@.exedat $^ 

.PHONY: tests
tests: DatabaseTests
	./$(TESTDIR)/DatabaseTests

