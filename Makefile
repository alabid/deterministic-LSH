CXX_OBJS = bin/main.o

CXX = clang++
OFLAGS = -O3
CXXFLAGS = -c -g -Wall -std=c++0x $(OFLAGS)

ifeq ($(shell which clang++),)
	CXX = g++
endif

MAIN := deterministic_lsh_main

all : $(MAIN)

$(MAIN) : $(CXX_OBJS)
	$(CXX) -o $@ $(CXX_OBJS)

bin/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean:
	rm -rf $(CXX_OBJS) $(ALL)
