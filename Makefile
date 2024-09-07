CXX = clang++ 
OFLAGS = # -O3 
FLAGS = -g `llvm-config --cxxflags --ldflags --libs core` -fsanitize=address

SRC_DIR = src
BUILD_DIR = build

OBJS = $(BUILD_DIR)/utils.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/irconstructor.o $(BUILD_DIR)/main.o

EXEC = $(BUILD_DIR)/compiler

all: $(EXEC)

# Link object files and create the final executable
$(EXEC): $(OBJS)
	$(CXX) $(OFLAGS) $(FLAGS) $(OBJS) -o $(EXEC)

# Compile each source file into an object file in the build directory
$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(SRC_DIR)/utils.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/utils.cpp -o $(BUILD_DIR)/utils.o

$(BUILD_DIR)/lexer.o: $(SRC_DIR)/lexer.cpp $(SRC_DIR)/lexer.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/lexer.cpp -o $(BUILD_DIR)/lexer.o

$(BUILD_DIR)/ast.o: $(SRC_DIR)/ast.cpp $(SRC_DIR)/lexer.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/ast.cpp -o $(BUILD_DIR)/ast.o

$(BUILD_DIR)/parser.o: $(SRC_DIR)/parser.cpp $(SRC_DIR)/lexer.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/parser.cpp -o $(BUILD_DIR)/parser.o

$(BUILD_DIR)/irconstructor.o: $(SRC_DIR)/IRConstructor.cpp $(SRC_DIR)/IRConstructor.hpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/IRConstructor.cpp -o $(BUILD_DIR)/irconstructor.o

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(OFLAGS) $(FLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
