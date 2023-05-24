#ifndef PPII2_GRP_30_COORDINATE_H
#define PPII2_GRP_30_COORDINATE_H

#include <stdbool.h>

typedef struct Coordinate {
    float latitude; // Latitude
    float longitude; // Longitude
} Coordinate;

int distance(Coordinate* coord1, Coordinate* coord2);
Coordinate* pos_after_step(Coordinate* coord1, Coordinate* coord2, int step_dist);
bool pos_equals(Coordinate* coord1, Coordinate* coord2);
#endif //PPII2_GRP_30_COORDINATE_H
