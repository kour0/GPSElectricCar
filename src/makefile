#LLVM=/opt/homebrew/opt/llvm
#CC=$(LLVM)/bin/clang
#CFLAGS= -Wall -Wextra -pedantic
#LDFLAGS=-L$(LLVM)/lib -fsanitize=address
#CPPFLAGS=-I$(LLVM)/include -Iinclude
#CFLAGS += $(CPPFLAGS) -O0 -g3 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls

CC=clang
CFLAGS=-Wall -Wextra -pedantic -O0 -g3 -fsanitize=address
LDFLAGS+=-L$lib -fsanitize=address
CPPFLAGS=-I$include -Iinclude

ALL_EXECUTABLES=main simu

all: $(ALL_EXECUTABLES)

main: main.o cJSON.o vehicle.o graph.o coordinate.o charging_station.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c constants.h structures/vehicle.h structures/graph.h structures/coordinate.h structures/charging_station.h
	$(CC) $(CFLAGS) -c -o $@ $<

simu: simu.o cJSON.o vehicle.o graph.o coordinate.o charging_station.o person.o queue.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

simu.o: simu.c
	$(CC) $(CFLAGS) -c -o $@ $<

cJSON.o: include/cJSON.c
	$(CC) $(CFLAGS) -c -o $@ $<
#STRUCTURES :
vehicle.o: structures/vehicle.c
	$(CC) $(CFLAGS) -c -o $@ $<

graph.o: structures/graph.c
	$(CC) $(CFLAGS) -c -o $@ $<

coordinate.o: structures/coordinate.c
	$(CC) $(CFLAGS) -c -o $@ $<

charging_station.o: structures/charging_station.c
	$(CC) $(CFLAGS) -c -o $@ $<

person.o: structures/person.c
	$(CC) $(CFLAGS) -c -o $@ $<

queue.o: structures/queue.c
	$(CC) $(CFLAGS) -c -o $@ $<



clean:
	rm -f *.o ${ALL_EXECUTABLES}
