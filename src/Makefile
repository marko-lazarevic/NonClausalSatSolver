CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS := -g -O0

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

.PHONY: all debug run clean

all: a.out

a.out: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS := $(filter-out -O2,$(CXXFLAGS)) $(DEBUG_FLAGS)
debug: clean a.out

run: a.out
	./a.out

clean:
	rm -f a.out $(OBJ)
