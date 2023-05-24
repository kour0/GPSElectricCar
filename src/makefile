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

ALL_EXECUTABLES=main simu test_queue test_stations

all: $(ALL_EXECUTABLES)

main: main.o cJSON.o vehicle.o graph.o coordinate.o charging_station.o queue.o person.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c constants.h structures/vehicle.h structures/graph.h structures/coordinate.h structures/charging_station.h structures/queue.h structures/person.h
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

#TESTS :

test : test_queue test_stations

test_queue: test_queue.o charging_station.o test_utils.o queue.o cJSON.o person.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_utils.o: ../test/test_utils.c
	$(CC) $(CFLAGS) -c -o $@ $<

test_queue.o: ../test/test_queue.c
	$(CC) $(CFLAGS) -c -o $@ $<

test_stations : test_stations.o charging_station.o test_utils.o cJSON.o person.o queue.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_stations.o: ../test/test_stations.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o ${ALL_EXECUTABLES}
