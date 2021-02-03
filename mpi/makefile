#
# 03/02/2021 Alessandro Basilici 1835392
#

CC	= mpicxx
CFLAGS	= -Wall
BASE	= crook
THREADED	= $(BASE)_threaded
OPTI	= _optimized
OPTIMIZE	= -O3
OUT	= _run

NUMTHREAD	= 3

all: compile run

compile:
	#$(CC) $(CFLAGS) -o $(BASE)$(OUT) $(BASE).cpp
	$(CC) $(CFLAGS) -o $(THREADED)$(OUT) $(THREADED).cpp
	$(CC) $(CFLAGS) $(OPTIMIZE) -o $(THREADED)$(OPTI)$(OUT) $(THREADED).cpp

clean:
	rm -f *$(OUT)

run:
	#sudo perf stat ./$(BASE)$(OUT)
	sudo perf stat mpiexec --allow-run-as-root -n $(NUMTHREAD) $(THREADED)$(OUT)
	sudo perf stat mpiexec --allow-run-as-root -n $(NUMTHREAD) $(THREADED)$(OPTI)$(OUT)
