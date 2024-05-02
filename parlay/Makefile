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
force_directed_drawing: force_directed_drawing.cpp force_directed_drawing.h
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ force_directed_drawing.cpp

clean:
	/bin/rm -rf *~ *.o force_directed_drawing
