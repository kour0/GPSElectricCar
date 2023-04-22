//
// Created by Noe Steiner on 22/04/2023.
//

#include "vehicle.h"
#include "../constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cJSON.h"
#include "../constants.h"

Vehicle* readJSONvehicles(char* filename, int* n) {
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
    cJSON* vehicles = cJSON_GetObjectItemCaseSensitive(json, "Vehicles");
    *n = cJSON_GetArraySize(vehicles);

    // Allocation du tableau de stations
    Vehicle* tab_vehicles = malloc(*n * sizeof(Vehicle));

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        cJSON* vehicle = cJSON_GetArrayItem(vehicles, i);
        cJSON* name = cJSON_GetObjectItemCaseSensitive(vehicle, "Name");
        cJSON* range = cJSON_GetObjectItemCaseSensitive(vehicle, "Range");
        cJSON* fastCharge = cJSON_GetObjectItemCaseSensitive(vehicle, "Fast Charge");

        tab_vehicles[i].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(tab_vehicles[i].name, name->valuestring);
        tab_vehicles[i].range = atoi(range->valuestring);
        tab_vehicles[i].fastCharge = atoi(fastCharge->valuestring);
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    return tab_vehicles;
}