SRCDIR = src
TESTDIR = tests
CC = g++ --std=c++17 -g -Wall

OBJECTS = $(SRCDIR)/ImmutDB.cpp $(SRCDIR)/Database.cpp
all: $(OBJECTS)
	$(CC) -o immutdb.exedat $? -lcaf_core -lrocksdb

$(TESTDIR)/DatabaseTests.o: $(TESTDIR)/DatabaseTests.cpp
	$(CC) -c -o $@ $? 

$(TESTDIR)/LayoutAccessTests.o: $(TESTDIR)/LayoutAccessTests.cpp
	$(CC) -c -o $@ $? 

$(TESTDIR)/KeyAccessTests.o: $(TESTDIR)/KeyAccessTests.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/Database.o: $(SRCDIR)/Database.cpp
	$(CC) -c -o $@ $? 

$(SRCDIR)/LayoutAccess.o: $(SRCDIR)/LayoutAccess.cpp 
	$(CC) -c -o $@ $? 

$(SRCDIR)/KeyAccess.o: $(SRCDIR)/KeyAccess.cpp 
	$(CC) -c -o $@ $? 

$(TESTDIR)/DatabaseTests: $(TESTDIR)/DatabaseTests.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

$(TESTDIR)/LayoutAccessTests: $(TESTDIR)/LayoutAccessTests.o $(SRCDIR)/LayoutAccess.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

$(TESTDIR)/KeyAccessTests: $(TESTDIR)/KeyAccessTests.o $(SRCDIR)/KeyAccess.o $(SRCDIR)/LayoutAccess.o $(SRCDIR)/Database.o
	$(CC) -o $@.exedat $? -lrocksdb

.PHONY: tests
tests: $(TESTDIR)/DatabaseTests $(TESTDIR)/LayoutAccessTests $(TESTDIR)/KeyAccessTests
	./$(TESTDIR)/DatabaseTests.exedat
	./$(TESTDIR)/LayoutAccessTests.exedat
	./$(TESTDIR)/KeyAccessTests.exedat

.PHONY: clean
clean:
	rm -f ./*.o
	rm -f ./*.exedat
	rm -f ./$(SRCDIR)/*.exedat
	rm -f ./$(TESTDIR)/*.exedat
	rm -f ./$(SRCDIR)/*.o
	rm -f ./$(TESTDIR)/*.o
