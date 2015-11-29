CXX_OBJS = bin/main.o

# modify to point to where where 'flann' header files and libraries are
FLANN_INCLUDES = -I/opt/local/include
FLANN_LINKS = -L/opt/local/lib

# modify to point to where lz4 is installed
LZ4_LIB = -L/usr/local/Cellar/lz4/r131/lib

CXX = clang++
OFLAGS = -O3
CXXFLAGS = -c -g -Wall -std=c++0x $(OFLAGS) $(FLANN_INCLUDES)
LDFLAGS = -g -Wall $(OFLAGS) $(FLANN_LINKS) $(LZ4_LIB) -lflann -lhdf5 -llz4

ifeq ($(shell which clang++),)
	CXX = g++
endif

MAIN := deterministic_lsh_main

all : $(MAIN)

$(MAIN) : $(CXX_OBJS)
	$(CXX) -o $@ $(CXX_OBJS) $(LDFLAGS)

bin/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean:
	rm -rf $(CXX_OBJS) $(ALL)
