//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_COORDINATE_H
#define PPII2_GRP_30_COORDINATE_H

typedef struct Coordinate {
    float latitude; // Latitude
    float longitude; // Longitude
} Coordinate;

float distance(Coordinate* coord1, Coordinate* coord2);
Coordinate* pos_after_step(Coordinate* coord1, Coordinate* coord2, float temps, float* remainingTime, int* path, int* pathSize);

#endif //PPII2_GRP_30_COORDINATE_H
