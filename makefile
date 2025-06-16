CC = gcc
CFLAGS = -Wall `sdl2-config --cflags` -Iinclude
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lSDL2_mixer

SRC = Src/main.c Src/character.c Src/animation.c Src/menu.c
OBJ = $(SRC:.c=.o)
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

Src/%.o: Src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f Src/*.o $(TARGET)
