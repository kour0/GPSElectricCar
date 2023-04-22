//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_CHARGING_STATION_H
#define PPII2_GRP_30_CHARGING_STATION_H
#include "coordinate.h"

typedef struct ChargingStation {
    char* name; // Nom de la station
    Coordinate coord; // Coordonnées géographiques
} ChargingStation;

ChargingStation* readJSONstations(char* filename, int* n, ChargingStation* depart, ChargingStation* arrivee);

#endif //PPII2_GRP_30_CHARGING_STATION_H
