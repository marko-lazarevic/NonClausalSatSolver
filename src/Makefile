CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS := -g -O0

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

# Parser files
PARSER_DIR := parser
PARSER_CPP := $(PARSER_DIR)/parser.cpp
PARSER_LEX := $(PARSER_DIR)/lexer.cpp
PARSER_SRC := $(PARSER_CPP) $(PARSER_LEX) $(PARSER_DIR)/fol.cpp
PARSER_OBJ := $(PARSER_SRC:.cpp=.o)

.PHONY: all debug run clean

all: a.out

a.out: $(OBJ) $(PARSER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate parser from bison
$(PARSER_CPP): $(PARSER_DIR)/parser.ypp
	bison -d -o $@ $<

# Generate lexer from flex
$(PARSER_LEX): $(PARSER_DIR)/lexer.lpp
	flex -o $@ $<

# Parser objects depend on generated files
$(PARSER_DIR)/parser.o: $(PARSER_CPP) $(PARSER_DIR)/parser.hpp
$(PARSER_DIR)/lexer.o: $(PARSER_LEX) $(PARSER_DIR)/parser.hpp
$(PARSER_DIR)/fol.o: $(PARSER_DIR)/fol.cpp $(PARSER_DIR)/fol.hpp

debug: CXXFLAGS := $(filter-out -O2,$(CXXFLAGS)) $(DEBUG_FLAGS)
debug: clean a.out

run: a.out
	./a.out

clean:
	rm -f a.out $(OBJ) $(PARSER_OBJ) $(PARSER_CPP) $(PARSER_DIR)/parser.hpp $(PARSER_LEX)
