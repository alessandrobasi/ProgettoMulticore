#
# 22/01/2021 Alessandro Basilici 1835392
#

CC	= g++
CFLAGS	= -Wall
BASE	= backtracking
THREADED	= $(BASE)_threaded
OPTI	= _optimized
LIST	= _list
OMPFLAG	= -fopenmp
OPTIMIZE	= -O3
OUT	= _run

NUMTHREAD	= 2

all: compile run

compile:
	$(CC) $(CFLAGS) -o $(BASE)$(OUT) $(BASE).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) -o $(THREADED)$(OUT) $(THREADED).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) -o $(THREADED)$(LIST)$(OUT) $(THREADED).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(OPTIMIZE) -o $(THREADED)$(OPTI)$(OUT) $(THREADED).cpp

clean:
	rm -f *$(OUT)

run:
	sudo perf stat ./$(BASE)$(OUT)
	sudo perf stat ./$(THREADED)$(OUT) $(NUMTHREAD)
	sudo perf stat ./$(THREADED)$(LIST)$(OUT) $(NUMTHREAD)
	sudo perf stat ./$(THREADED)$(OPTI)$(OUT) $(NUMTHREAD)
