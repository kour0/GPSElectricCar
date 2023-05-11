//
// Created by kour0 on 5/10/23.
//

#include "person.h"

float timeToFastCharge(Person* person, int distance){
    Vehicle* vehicle = person->vehicle;
    float fastCharge = vehicle->fastCharge;
    float autonomy = person->autonomy;
    float temps = ((distance-autonomy) / fastCharge) * 60; // en minutes
    return temps;
}
