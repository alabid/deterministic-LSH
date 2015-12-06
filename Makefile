CXX_OBJS_LIN = bin/linear_scan.o
CXX_OBJS_FLANN = bin/flann.o
CXX_OBJS = bin/*.o

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

FLANN_LSH := flann_lsh_main
LINEAR_SCAN := linear_scan_main

all : $(FLANN_LSH) $(LINEAR_SCAN)

$(FLANN_LSH) : $(CXX_OBJS_FLANN)
	$(CXX) -o $@ $(CXX_OBJS_FLANN) $(LDFLAGS)

$(LINEAR_SCAN) : $(CXX_OBJS_LIN)
	$(CXX) -o $@ $(CXX_OBJS_LIN)

bin/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean:
	rm -rf $(CXX_OBJS) $(ALL)
