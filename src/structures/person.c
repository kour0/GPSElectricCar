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
    person->remainingAutonomy = vehicle->range;
    person->remainingTime = 0;
    person->end = end;
    return person;
}

int timeToFastCharge(Person* person, int distance){
    int fastCharge = person->vehicle->fastCharge;
    int autonomy = person->remainingAutonomy;
    int temps = ((distance-autonomy) / fastCharge);
    return temps;

}
