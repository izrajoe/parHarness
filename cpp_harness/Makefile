IDIR =./
CC=g++

# -DLEVEL1_DCACHE_LINESIZE detects the cache line size and passes it in as a compiler flag

CFLAGS=-I$(IDIR) -I ./include -m32 -Wno-write-strings -fpermissive -pthread -std=c++0x -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE`

# Additional options for different builds:

# gcov build
# -fprofile-arcs -ftest-coverage -O0 
# test build
#-DNDEBUG -O3  
#debug build
#-g -pg -rdynamic 
# line by line debug coverage (access via command line: gprof -l)
#-O0 -pg -g 

CFLAGS+= -O3 -g

ODIR=./obj
LDIR =./

LIBS=-lpthread 

_DEPS = HarnessUtils.hpp ParallelLaunch.hpp RContainer.hpp TestConfig.hpp DefaultHarnessTests.hpp SGLQueue.hpp HazardTracker.hpp RMap.hpp ConcurrentPrimitives.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = ParallelLaunch.o TestConfig.o DefaultHarnessTests.o SGLQueue.o HarnessUtils.o Recorder.o HazardTracker.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

all: harness library

harness: $(ODIR)/Harness.o  $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

library: $(OBJ)
	ar rcs libharness.a $(OBJ)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ harness libharness.a

