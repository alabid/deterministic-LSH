CXX_OBJS_DETERM_LSH = bin/deterministic_lsh.o
CXX_OBJS_RANDOM_LSH = bin/randomized_lsh.o
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
CXXFLAGS = -c -Wall -std=c++11 $(OFLAGS) $(FLANN_INCLUDES)
LDFLAGS = -Wall $(OFLAGS) $(FLANN_LINKS) $(LZ4_LIB) -lflann

ifeq ($(shell which clang++),)
	CXX = g++
endif

FLANN_LSH := flann_lsh_main
LINEAR_SCAN := linear_scan_main
RANDOMIZED_LSH := randomized_lsh_main
DETERMINISTIC_LSH := deterministic_lsh_main

all : $(FLANN_LSH) $(LINEAR_SCAN) $(RANDOMIZED_LSH) $(DETERMINISTIC_LSH)

$(FLANN_LSH) : $(CXX_OBJS_FLANN)
	$(CXX) -o $@ $(CXX_OBJS_FLANN) $(LDFLAGS)

$(LINEAR_SCAN) : $(CXX_OBJS_LIN)
	$(CXX) -o $@ $(CXX_OBJS_LIN)

$(RANDOMIZED_LSH) : $(CXX_OBJS_RANDOM_LSH)
	$(CXX) -o $@ $(CXX_OBJS_RANDOM_LSH)

$(DETERMINISTIC_LSH) : $(CXX_OBJS_DETERM_LSH)
	$(CXX) -o $@ $(CXX_OBJS_DETERM_LSH)

bin/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean:
	rm -rf $(CXX_OBJS) *_main
