//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_COORDINATE_H
#define PPII2_GRP_30_COORDINATE_H
#include <stdbool.h>

typedef struct Coordinate {
    float latitude; // Latitude
    float longitude; // Longitude
} Coordinate;

bool isInCircle(Coordinate coord, double longitude, double latitude, double r);
double distance(Coordinate coord1, Coordinate coord2);

#endif //PPII2_GRP_30_COORDINATE_H
