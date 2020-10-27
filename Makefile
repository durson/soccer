CXX         ?= g++
CXXFLAGS     = $(INCLUDES_DIR) -std=c++11 -Wall -Wextra -Wpedantic -march=native -mavx2 -Ofast -pipe -flto -fomit-frame-pointer -ftree-loop-im -funroll-loops -fPIC
INCLUDES_DIR = -Iinclude -I.
BIN_DIR      = bin
OBJ_DIR      = obj
SRC_DIR      = src
TARGETS      = bin/soccer bin/human.so bin/random.so
LIBS         = -ldl

SOURCES := $(shell find $(SRC_DIR) -name *.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

.PHONY: all clean $(TARGETS)

all: $(TARGETS)

bin/soccer: $(addprefix $(OBJ_DIR)/, soccer.o) $(addprefix $(BIN_DIR)/, board.so)
	mkdir -p $(BIN_DIR)
	$(CXX) $^ $(LIBS) $(CXXFLAGS) -o $@

bin/human.so: $(addprefix $(OBJ_DIR)/, controller/human.o)
	mkdir -p $(BIN_DIR)
	$(CXX) -shared $^ $(LIBS) $(CXXFLAGS) -o $@

bin/random.so: $(addprefix $(OBJ_DIR)/, controller/random.o)
	mkdir -p $(BIN_DIR)
	$(CXX) -shared $^ $(LIBS) $(CXXFLAGS) -o $@

bin/board.so: $(addprefix $(OBJ_DIR)/, board.o)
	mkdir -p $(BIN_DIR)
	$(CXX) -shared $^ $(LIBS) $(CXXFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	$(CXX) $(CXXFLAGS) -MM $< | sed -r 's|.*:|$@:|' > $@.d #generate dependency file

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

-include $(OBJECTS:=.d) #pull in dependencies
