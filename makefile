CC=clang
CFLAGS=-Wall -Wextra -pedantic -O0 -g3 -fsanitize=address
LDFLAGS+=-fsanitize=address

ALL_EXECUTABLES=main

all: $(ALL_EXECUTABLES)

main: main.o cJSON.o

main.o: main.c

cJSON.o: cJSON.c

clean: 
	rm -f *.o ${ALL_EXECUTABLES}