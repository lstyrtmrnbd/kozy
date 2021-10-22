CXX = g++

CXXFLAGS = -std=c++2a -ggdb -Wall -Wextra -Wpedantic -pthread

LINK = -lstdc++ -lsfml-graphics -lsfml-window -lsfml-system -lchibi-scheme -pthread

SOURCES = $(wildcard src/*.cpp)
DEPS = $(wildcard srd/*.h*)
OBJ = $(SOURCES:.cpp=.o)

src/chub.c: src/chub.stub
	chibi-ffi $^

src/chub.o: src/chub.c
	$(CXX) $(FLAGS) $^ -lchibi-scheme

kozy: $(OBJ) $(DEPS)
	$(CXX) $(FLAGS) -o $@ $^ $(LINK)

.PHONY: clean

clean:
	rm $(OBJ)
