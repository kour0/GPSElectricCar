#include "vehicle.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cJSON.h"

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
        tab_vehicles[i].range = atoi(range->valuestring)*1000;
        tab_vehicles[i].fastCharge =  (int) round(atoi(fastCharge->valuestring) / 3.6);
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    return tab_vehicles;
}

// Fonction qui sérialise un tableau de stations en fichier binaire
void serializeVehicles(char* filename, Vehicle* vehicles, int n) {
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
        int size = (int)strlen(vehicles[i].name);
        fwrite(&size, sizeof(int), 1, file);

        // Ecriture du nom
        fwrite(vehicles[i].name, sizeof(char), size, file);

        // Ecriture Fast Charge
        fwrite(&vehicles[i].fastCharge, sizeof(int), 1, file);

        // Ecriture de la range
        fwrite(&vehicles[i].range, sizeof(float), 1, file);
    }

    // Fermeture du fichier
    fclose(file);

    printf("Stations sérialisées dans le fichier %s\n", filename);
}

Vehicle* deserializeVehicles(char* filename, int* n){
    // Ouverture du fichier
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Lecture du nombre de stations
    fread(n, sizeof(int), 1, file);

    // Allocation du tableau de stations
    Vehicle* vehicles = malloc(*n * sizeof(Vehicle));

    // Lecture des stations
    for (int i = 0; i < *n; ++i) {
        // Lecture de la taille du nom
        int size;
        fread(&size, sizeof(int), 1, file);

        // Allocation du nom
        vehicles[i].name = malloc((size + 1) * sizeof(char));

        // Lecture du nom
        fread(vehicles[i].name, sizeof(char), size, file);
        vehicles[i].name[size] = 0;

        // Lecture de fast charge
        fread(&(vehicles[i].fastCharge), sizeof(int), 1, file);

        // Lecture de la range
        fread(&(vehicles[i].range), sizeof(float), 1, file);
    }

    // Fermeture du fichier
    fclose(file);

    printf("Stations désérialisées depuis le fichier %s\n", filename);

    return vehicles;
}
