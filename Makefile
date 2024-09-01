CXX = clang++ 
OFLAGS = # -O3 
FLAGS = -g `llvm-config --cxxflags --ldflags --libs core` -fsanitize=address

OBJS = lexer.o ast.o parser.o main.o
EXEC = compiler

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OFLAGS) $(FLAGS) $(OBJS) -o $(EXEC)

lexer.o: lexer.cpp lexer.hpp
	$(CXX) $(OFLAGS) $(FLAGS) -c lexer.cpp -o lexer.o

ast.o: ast.cpp lexer.hpp
	$(CXX) $(OFLAGS) $(FLAGS) -c ast.cpp -o ast.o

parser.o: parser.cpp lexer.hpp
	$(CXX) $(OFLAGS) $(FLAGS) -c parser.cpp -o parser.o

main.o: main.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c main.cpp -o main.o

clean:
	rm -f $(OBJS) $(EXEC)
