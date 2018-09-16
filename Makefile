SRCDIR = src
TESTDIR = tests
CC = g++ --std=c++17 -g -Wall

OBJECTS = $(SRCDIR)/ImmutDB.cpp $(SRCDIR)/Database.cpp
all: $(OBJECTS)
	$(CC) -o immutdb.exedat $? -lcaf_core -lrocksdb

$(TESTDIR)/DatabaseTests.o: $(TESTDIR)/DatabaseTests.cpp
	$(CC) -c -o $@ $? 

$(TESTDIR)/SchemaTests.o: $(TESTDIR)/SchemaTests.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/Database.o: $(SRCDIR)/Database.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/Schema.o: $(SRCDIR)/Schema.cpp $(SRCDIR)/Database.o
	$(CC) -c -o $@ $? 

$(TESTDIR)/DatabaseTests: $(TESTDIR)/DatabaseTests.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

$(TESTDIR)/SchemaTests: $(TESTDIR)/SchemaTests.o $(SRCDIR)/Schema.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

.PHONY: tests
tests: $(TESTDIR)/DatabaseTests $(TESTDIR)/SchemaTests
	./$(TESTDIR)/DatabaseTests.exedat
	./$(TESTDIR)/SchemaTests.exedat

.PHONY: clean
clean:
	rm -f ./*.o
	rm -f ./*.exedat
	rm -f ./$(SRCDIR)/*.exedat
	rm -f ./$(TESTDIR)/*.exedat
	rm -f ./$(SRCDIR)/*.o
	rm -f ./$(TESTDIR)/*.o
