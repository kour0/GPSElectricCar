#ifndef PPII2_GRP_30_VEHICLE_H
#define PPII2_GRP_30_VEHICLE_H

typedef struct Vehicle {
    char* name; // Nom du véhicule
    int fastCharge; // Capacité de charge rapide en m/s
    int range; // Autonomie
} Vehicle;

Vehicle* readJSONvehicles(char* filename, int* n);
void serializeVehicles(char* filename, Vehicle* vehicles, int n);
Vehicle* deserializeVehicles(char* filename, int* n);

#endif //PPII2_GRP_30_VEHICLE_H
