//
// Created by Noe Steiner on 22/04/2023.
//

#include "coordinate.h"
#include <math.h>
#include "../constants.h"
#include <stdlib.h>

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


// Fonction qui calcule la position après un déplacement de distance sur un chemin
Coordinate* pos_after_step(Coordinate* coord1, Coordinate* coord2, float distance) {

    double d = distance(coord1, coord2);

    // Calcul de la position après un déplacement de distance sur le chemin
    double A = sin((1 - distance / d) * c) / sin(c);
    double B = sin(distance / d * c) / sin(c);
    double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
    double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
    double z = A * sin(lat1) + B * sin(lat2);
    double lat = atan2(z, sqrt(pow(x, 2) + pow(y, 2)));
    double lon = atan2(y, x);

    // Conversion des coordonnées en degrés
    lat = lat * 180 / M_PI;
    lon = lon * 180 / M_PI;

    // Création de la nouvelle coordonnée
    Coordinate* coord = malloc(sizeof(Coordinate));
    coord->latitude = lat;
    coord->longitude = lon;

    return coord;
}
