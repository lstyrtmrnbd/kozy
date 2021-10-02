CXX = g++

FLAGS = -std=c++17 -ggdb -Wall -Wextra -Wpedantic 

LINK = -lstdc++ -lsfml-graphics -lsfml-window -lsfml-system -lchibi-scheme

SOURCES = $(wildcard src/*.cpp)

OBJ = $(SOURCES:.cpp=.o)

all: $(OBJ)
	$(CXX) $(FLAGS) -o kozy.bin $^ $(LINK)

clean:
	rm $(OBJ)
