//
// Created by kour0 on 5/10/23.
//

#include "person.h"
#include "../constants.h"
#include "coordinate.h"
#include <stdlib.h>
#include "charging_station.h"
#include "graph.h"

float timeToFastCharge(Person* person, float distance){
    Vehicle* vehicle = person->vehicle;
    float fastCharge = vehicle->fastCharge;
    float autonomy = person->autonomy;
    float temps = ((distance-autonomy) / fastCharge) * 3600; // en secondes
    return temps;
}

void nextStep(Person* person, Coordinate* next_station) {
    // On est arrivé à destination
    if (person->pathSize == 1) {
        return;
    }
    // On est sur un chemin
    if (person->remainingTime == 0) {
        Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP, &(person->remainingTime), person->path, &(person->pathSize));
        free(person->coordinate);
        if (next_station->longitude == new_coord->longitude && next_station->latitude == new_coord->latitude) {
            // On est arrivé
            person->pathSize = 1;
        }
        person->coordinate = new_coord;
    } else {
        // On est dans une station
        if (person->remainingTime - STEP >= 0) {
            person->remainingTime -= STEP;
        }
        else {
            // On sort de la station avec une distance en plus
            Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP - person->remainingTime, &(person->remainingTime), person->path, &(person->pathSize));
            free(person->coordinate);
            person->coordinate = new_coord;
            person->remainingTime = 0;
        }
    }
}
