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

        //if (isInCircle((Coordinate) {longitude->valuedouble, latitude->valuedouble}, center_longitude, center_latitude, rayon)) {
        if (1) {
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
    // stations = realloc(stations, size * sizeof(ChargingStation));

    // *n = size;

    return stations;
}

void serializeStations(char* filename, ChargingStation* stations, int n) {
    // Ouverture du fichier
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Ecriture du nombre de stations
    fwrite(&n, sizeof(int), 1, file);

    // Ecriture des stations
    for (int i = 0; i < n; ++i) {
        // Ecriture de la taille du nom
        int size = strlen(stations[i].name);
        fwrite(&size, sizeof(int), 1, file);

        // Ecriture du nom
        fwrite(stations[i].name, sizeof(char), size, file);

        // Ecriture de la longitude
        fwrite(&stations[i].coord.longitude, sizeof(float), 1, file);

        // Ecriture de la latitude
        fwrite(&stations[i].coord.latitude, sizeof(float), 1, file);
    }

    // Fermeture du fichier
    fclose(file);

    printf("Stations sérialisées dans le fichier %s\n", filename);
}

ChargingStation* deserializeStations(char* filename, int* n) {
    // Ouverture du fichier
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Lecture du nombre de stations
    fread(n, sizeof(int), 1, file);

    // Allocation du tableau de stations
    ChargingStation* stations = malloc(*n * sizeof(ChargingStation));

    // Lecture des stations
    for (int i = 0; i < *n; ++i) {
        // Lecture de la taille du nom
        int size;
        fread(&size, sizeof(int), 1, file);

        // Allocation du nom
        stations[i].name = malloc((size + 1) * sizeof(char));

        // Lecture du nom
        fread(stations[i].name, sizeof(char), size, file);
        stations[i].name[size] = 0;

        // Lecture de la longitude
        fread(&stations[i].coord.longitude, sizeof(float), 1, file);

        // Lecture de la latitude
        fread(&stations[i].coord.latitude, sizeof(float), 1, file);
    }

    // Fermeture du fichier
    fclose(file);

    printf("Stations désérialisées depuis le fichier %s\n", filename);

    return stations;
}
