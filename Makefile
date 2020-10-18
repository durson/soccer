CXX         ?= g++
CXXFLAGS     = $(INCLUDES_DIR) -std=c++11 -Wall -Wextra -Wpedantic -march=native -mavx2 -Ofast -pipe -flto -fomit-frame-pointer -ftree-loop-im -funroll-loops
INCLUDES_DIR = -Iinclude
BIN_DIR      = bin
OBJ_DIR      = obj
SRC_DIR      = src
TARGETS      = soccer
LIBS         = 

.PHONY: all clean

all: $(TARGETS)

soccer: $(addprefix $(OBJ_DIR)/, main.o)
	mkdir -p $(BIN_DIR)
	$(CXX) $^ $(LIBS) $(CXXFLAGS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
