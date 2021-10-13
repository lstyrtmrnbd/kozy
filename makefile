CXX = g++

FLAGS = -std=c++17 -ggdb -Wall -Wextra -Wpedantic 

LINK = -lstdc++ -lsfml-graphics -lsfml-window -lsfml-system -lchibi-scheme

SOURCES = $(wildcard src/*.cpp)

OBJ = $(SOURCES:.cpp=.o)

src/chub.c: src/chub.stub
	chibi-ffi $^

src/chub.o: src/chub.c
	$(CXX) $(FLAGS) $^ -lchibi-scheme

all: $(OBJ)
	$(CXX) $(FLAGS) -o kozy.bin $^ $(LINK)

clean:
	rm $(OBJ)
