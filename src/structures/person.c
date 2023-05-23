//
// Created by kour0 on 5/10/23.
//

#include "person.h"
#include "coordinate.h"
#include <stdlib.h>

Person* createPerson(Vehicle* vehicle, Coordinate* coordinate, int* path, int pathSize, Coordinate* end) {

    Person* person = malloc(sizeof(Person));
    person->vehicle = vehicle;
    person->coordinate = coordinate;
    person->path = path;
    person->pathSize = pathSize;
    person->autonomy = vehicle->range;
    person->remainingTime = 0;
    person->end = end;
    return person;
}

float timeToFastCharge(Person* person, float distance){
    Vehicle* vehicle = person->vehicle;
    float fastCharge = vehicle->fastCharge;
    float autonomy = person->autonomy;
    float temps = ((distance-autonomy) / fastCharge) * 3600; // en secondes
    return temps;
}
