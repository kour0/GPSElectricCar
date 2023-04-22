//
// Created by Noe Steiner on 22/04/2023.
//

#include "charging_station.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cJSON.h"
#include "../constants.h"

ChargingStation* readJSONstations(char* filename, int* n, ChargingStation* depart, ChargingStation* arrivee) {
    // Ouverture du fichier
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Lecture du fichier
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* string = malloc(fsize + 1);
    fread(string, 1, fsize, file);
    fclose(file);
    string[fsize] = 0;

    cJSON* json = cJSON_Parse(string);
    free(string);
    if (json == NULL) {
        printf("Erreur lors du parsing du fichier JSON\n");
        exit(1);
    }

    // Récupération du nombre de stations
    cJSON* features = cJSON_GetObjectItemCaseSensitive(json, "stations");
    *n = cJSON_GetArraySize(features);

    // Allocation du tableau de stations
    ChargingStation* stations = malloc(*n * sizeof(ChargingStation));

    int size = 0;

    double center_longitude = (depart->coord.longitude + arrivee->coord.longitude) / 2;
    double center_latitude = (depart->coord.latitude + arrivee->coord.latitude) / 2;
    double rayon = distance(depart->coord, arrivee->coord) / 2;

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        // On prend que les stations qui nous intéresse en prenant comme centre du cercle le milieu des deux stations de départ et d'arrivée
        cJSON* feature = cJSON_GetArrayItem(features, i);
        cJSON* name = cJSON_GetObjectItemCaseSensitive(feature, "nom_station");
        cJSON* longitude = cJSON_GetObjectItemCaseSensitive(feature, "longitude");
        cJSON* latitude = cJSON_GetObjectItemCaseSensitive(feature, "latitude");

        if (isInCircle((Coordinate) {longitude->valuedouble, latitude->valuedouble}, center_longitude, center_latitude, rayon)) {
            stations[size].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
            strcpy(stations[size].name, name->valuestring);
            stations[size].coord.longitude = longitude->valuedouble;
            stations[size].coord.latitude = latitude->valuedouble;
            size++;
        }
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    // Realloc du tableau de stations
    stations = realloc(stations, size * sizeof(ChargingStation));

    *n = size;

    return stations;
}
