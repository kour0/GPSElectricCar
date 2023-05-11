//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_VEHICLE_H
#define PPII2_GRP_30_VEHICLE_H

typedef struct Vehicle {
    char* name; // Nom du véhicule
    int fastCharge; // Capacité de charge rapide en km/h
    int range; // Autonomie
} Vehicle;

Vehicle* readJSONvehicles(char* filename, int* n);

#endif //PPII2_GRP_30_VEHICLE_H
