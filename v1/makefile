CC=clang
CFLAGS=-Wall -Wextra -pedantic -O0 -g3 -fsanitize=address
LDFLAGS+=-fsanitize=address -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

ALL_EXECUTABLES=main

all: $(ALL_EXECUTABLES)

main: main.o cJSON.o fonctions.o

main.o: main.c

cJSON.o: cJSON.c

fonctions.o: fonctions.c

clean: 
	rm -f *.o ${ALL_EXECUTABLES}