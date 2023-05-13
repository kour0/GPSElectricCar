//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_CHARGING_STATION_H
#define PPII2_GRP_30_CHARGING_STATION_H

#include "coordinate.h"
#include "queue.h"

typedef struct ChargingStation {
    char* name; // Nom de la station
    Coordinate* coord; // Coordonnées géographiques
    int nbChargingPoints; // Nombre de points de charge
    int nbAvailableChargingPoints; // Nombre de points de charge disponibles
    Queue* queue; // Liste des places
} ChargingStation;

ChargingStation* readJSONstations(char* filename, int* n);
void serializeStations(char* filename, ChargingStation* stations, int n);
ChargingStation* deserializeStations(char* filename, int* n);
void addPersonToStation(ChargingStation* station, ChargingStation* stations, Person* person);

#endif //PPII2_GRP_30_CHARGING_STATION_H
