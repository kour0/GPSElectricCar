//
// Created by Noe Steiner on 22/04/2023.
//

#include "coordinate.h"
#include <math.h>
#include "../constants.h"

bool isInCircle(Coordinate coord, double longitude, double latitude, double r) {
    double d = distance(coord, (Coordinate) {longitude, latitude});
    return d <= r;
}

// Fonction pour calculer la distance entre deux coordonnées géographiques en km
double distance(Coordinate coord1, Coordinate coord2) {
    // Conversion des coordonnées en radians
    double lat1 = coord1.latitude * M_PI / 180;
    double lon1 = coord1.longitude * M_PI / 180;
    double lat2 = coord2.latitude * M_PI / 180;
    double lon2 = coord2.longitude * M_PI / 180;

    // Calcul de la distance
    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return (EARTH_RADIUS * c);
}