//
// Created by kour0 on 5/10/23.
//

#include "person.h"
#include "../constants.h"
#include "coordinate.h"
#include <stdlib.h>

float timeToFastCharge(Person* person, int distance){
    Vehicle* vehicle = person->vehicle;
    float fastCharge = vehicle->fastCharge;
    float autonomy = person->autonomy;
    float temps = ((distance-autonomy) / fastCharge) * 3600; // en secondes
    return temps;
}

void nextStepDistance(Person* person, int distance) {
    if (person->RemainingTime - STEP >= 0) {
        person->RemainingTime -= STEP;
        person->distance += STEP * person->vehicle->speed;
        if (person->distance >= distance) {
            person->distance = distance;
            person->RemainingTime = 0;
        }
    }
}

void nextStep(Person* person, ChargingStation* stations) {
    if (person->RemainingTime == -1) {
        Coordinate* new_coord = pos_after_step(person->coordinate, stations[person->path[1]], STEP);
        free(person->coordinate);
        person->coordinate = new_coord;
    } else {
        if (person->remainingTime - STEP > 0) {
            person->remainingTime -= STEP;
        }
        else {
            Coordinate* new_coord = pos_after_step(person->coordinate, stations[person->path[1]], STEP - person->remainingTime);
            free(person->coordinate);
            person->coordinate = new_coord;
            person->remainingTime = -1;
        }
    }
}
