#ifndef PPII2_GRP_30_PERSON_H
#define PPII2_GRP_30_PERSON_H

#include "vehicle.h"
#include "coordinate.h"


typedef struct Person {
    Vehicle* vehicle;
    Coordinate* coordinate;
    int* path;
    int pathSize;
    int waitingTime;
    int chargingTime;
    int remainingAutonomy;
    Coordinate* end;
} Person;

Person* createPerson(Vehicle* vehicle, Coordinate* coordinate, int* path, int pathSize, Coordinate* end);
int timeToFastCharge(Person* person, int distance);

#endif //PPII2_GRP_30_PERSON_H
