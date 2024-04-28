ifeq (, $(shell which jemalloc-config))
JEMALLOC =
else
JEMALLOC = -L$(JEMALLOCLD) -ljemalloc 
endif
# TODO get jemalloc working

CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread -I parlaylib/include/ -I parlaylib/examples/helper -Wno-unknown-pragmas $(JEMALLOC)
OPTFLAGS = -O3

PARLAYLIB = parlaylib/
calculate_pos: calculate_pos.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ calculate_pos.cpp

clean:
	/bin/rm -rf *~ *.o calculate_pos
