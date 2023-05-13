//
// Created by kour0 on 5/10/23.
//

#ifndef PPII2_GRP_30_PERSON_H
#define PPII2_GRP_30_PERSON_H

#include "vehicle.h"
#include "coordinate.h"


typedef struct Person {
    Vehicle* vehicle;
    Coordinate* coordinate;
    int* path;
    int pathSize;
    float remainingTime;
    float autonomy;
} Person;

float timeToFastCharge(Person* person, float distance);
void nextStep(Person* person, Coordinate* next_station);

#endif //PPII2_GRP_30_PERSON_H
