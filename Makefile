CXX = clang++ 
OFLAGS = -O3 
FLAGS = -std=c++20 -Wall -Wno-unused-private-field -fsanitize=address

OBJS = lexer.o ast.o parser.o main.o
EXEC = compiler

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OFLAGS) $(FLAGS) $(OBJS) -o $(EXEC)

lexer.o: lexer.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c lexer.cpp -o lexer.o

ast.o: ast.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c ast.cpp -o ast.o

parser.o: parser.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c parser.cpp -o parser.o

main.o: main.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c main.cpp -o main.o

clean:
	rm -f $(OBJS) $(EXEC)
