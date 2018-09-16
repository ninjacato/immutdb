SRCDIR = src
TESTDIR = tests
CC = g++ --std=c++17 -g -Wall

OBJECTS = $(SRCDIR)/ImmutDB.cpp $(SRCDIR)/Database.cpp
all: $(OBJECTS)
	$(CC) -o immutdb.exedat $? -lcaf_core -lrocksdb

$(TESTDIR)/DatabaseTests.o: $(TESTDIR)/DatabaseTests.cpp
	$(CC) -c -o $@ $? 

$(TESTDIR)/LayoutTests.o: $(TESTDIR)/LayoutTests.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/Database.o: $(SRCDIR)/Database.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/Layout.o: $(SRCDIR)/Layout.cpp 
	$(CC) -c -o $@ $? 

$(TESTDIR)/DatabaseTests: $(TESTDIR)/DatabaseTests.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

$(TESTDIR)/LayoutTests: $(TESTDIR)/LayoutTests.o $(SRCDIR)/Layout.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

.PHONY: tests
tests: $(TESTDIR)/DatabaseTests $(TESTDIR)/LayoutTests
	./$(TESTDIR)/DatabaseTests.exedat
	./$(TESTDIR)/LayoutTests.exedat

.PHONY: clean
clean:
	rm -f ./*.o
	rm -f ./*.exedat
	rm -f ./$(SRCDIR)/*.exedat
	rm -f ./$(TESTDIR)/*.exedat
	rm -f ./$(SRCDIR)/*.o
	rm -f ./$(TESTDIR)/*.o
