CC = g++
CFLAGS = --std=c++23 -Wall -Wextra
LIBFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lvlc


FINAL = prog
SRC_DIR=src
SOURCES=$(wildcard $(SRC_DIR)/*/*.cpp)

all: $(SRC_DIR)
	$(CC) $(CFLAGS) -o $(FINAL) $(SOURCES) $(LIBFLAGS)

clean:
	rm -f *~
	rm -f *.o
	rm -f $(NAME)
