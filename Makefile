CXX = clang++ 
OFLAGS = # -O3 
FLAGS = -g `llvm-config --cxxflags --ldflags --libs core` -fsanitize=address

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
UNIT_TEST_DIR = tests/unit

OBJS = $(BUILD_DIR)/utils.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/irconstructor.o

################ ------------ Main Executeable ------------ ################

EXEC = $(BUILD_DIR)/compiler

all: $(EXEC)

# Link object files and create the final executable
$(EXEC): $(OBJS) $(BUILD_DIR)/main.o
	$(CXX) $(OFLAGS) $(FLAGS) $(OBJS) $(BUILD_DIR)/main.o -o $(EXEC)

# Compile each source file into an object file in the build directory
$(BUILD_DIR)/utils.o: $(SRC_DIR)/Utils.cpp $(INCLUDE_DIR)/Utils.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/Utils.cpp -o $(BUILD_DIR)/utils.o

$(BUILD_DIR)/lexer.o: $(SRC_DIR)/Lexer.cpp $(INCLUDE_DIR)/Lexer.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/Lexer.cpp -o $(BUILD_DIR)/lexer.o

$(BUILD_DIR)/ast.o: $(SRC_DIR)/AST.cpp $(INCLUDE_DIR)/AST.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/AST.cpp -o $(BUILD_DIR)/ast.o

$(BUILD_DIR)/parser.o: $(SRC_DIR)/Parser.cpp $(INCLUDE_DIR)/Parser.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/Parser.cpp -o $(BUILD_DIR)/parser.o

$(BUILD_DIR)/irconstructor.o: $(SRC_DIR)/IRConstructor.cpp $(INCLUDE_DIR)/IRConstructor.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/IRConstructor.cpp -o $(BUILD_DIR)/irconstructor.o

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

################ ------------ TESTS ------------ ################
GTEST_DIR = /usr/local
GTEST_INC = -I$(GTEST_DIR)/include
GTEST_LIB = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread
GTEST_RPATH = -Wl,-rpath,$(GTEST_DIR)/lib

TEST_OBJS = $(BUILD_DIR)/testParser.o 

test: $(OBJS) $(TEST_OBJS) $(BUILD_DIR)/runner.o
	$(CXX) $(OFLAGS) $(FLAGS) $(GTEST_LIB) $(GTEST_RPATH) $(OBJS) $(TEST_OBJS) $(BUILD_DIR)/runner.o -o $(BUILD_DIR)/test-runner

$(BUILD_DIR)/testParser.o: $(UNIT_TEST_DIR)/testParser.cpp
	$(CXX) $(OFLAGS)  $(FLAGS) $(GTEST_INC) -c $(UNIT_TEST_DIR)/testParser.cpp -o $(BUILD_DIR)/testParser.o

$(BUILD_DIR)/runner.o: $(UNIT_TEST_DIR)/runner.cpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) $(GTEST_INC) -c $(UNIT_TEST_DIR)/runner.cpp -o $(BUILD_DIR)/runner.o

clean:
	rm -rf $(BUILD_DIR)
