#include "charging_station.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cJSON.h"
#include "queue.h"

ChargingStation* readJSONstations(char* filename, int* n) {
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

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        // On prend que les stations qui nous intéresse en prenant comme centre du cercle le milieu des deux stations de départ et d'arrivée
        cJSON* feature = cJSON_GetArrayItem(features, i);
        cJSON* name = cJSON_GetObjectItemCaseSensitive(feature, "nom_station");
        cJSON* longitude = cJSON_GetObjectItemCaseSensitive(feature, "longitude");
        cJSON* latitude = cJSON_GetObjectItemCaseSensitive(feature, "latitude");
        cJSON* nbre_places = cJSON_GetObjectItemCaseSensitive(feature, "nbre_pdc");

        //if (isInCircle((Coordinate) {longitude->valuedouble, latitude->valuedouble}, center_longitude, center_latitude, rayon)) {
        stations[i].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(stations[i].name, name->valuestring);
        stations[i].coord = malloc(sizeof(Coordinate));
        stations[i].coord->longitude = (float)longitude->valuedouble;
        stations[i].coord->latitude = (float)latitude->valuedouble;
        stations[i].nbChargingPoints = atoi(nbre_places->valuestring);
        stations[i].nbAvailableChargingPoints = atoi(nbre_places->valuestring);

        stations[i].queues = malloc(sizeof(Queue*) * stations[i].nbChargingPoints);

        for (int j = 0; j < stations[i].nbChargingPoints; ++j) {
            stations[i].queues[j] = createQueue();
        }
    }

    // Libération de la mémoire
    cJSON_Delete(json);

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
        int size = (int)strlen(stations[i].name);
        fwrite(&size, sizeof(int), 1, file);

        // Ecriture du nom
        fwrite(stations[i].name, sizeof(char), size, file);

        // Ecriture du nombre de places
        fwrite(&stations[i].nbChargingPoints, sizeof(int), 1, file);

        // Ecriture du nombre de places
        fwrite(&stations[i].nbAvailableChargingPoints, sizeof(int), 1, file);

        // Ecriture de la longitude
        fwrite(&stations[i].coord->longitude, sizeof(float), 1, file);

        // Ecriture de la latitude
        fwrite(&stations[i].coord->latitude, sizeof(float), 1, file);

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
        stations[i].coord = malloc(sizeof(Coordinate));

        for (int j = 0; j < size; ++j) {
            stations[i].name[j] = 0;
        }

        // Lecture du nom
        fread(stations[i].name, sizeof(char), size, file);
        stations[i].name[size] = 0;

        // Lecture du nombre de places
        fread(&(stations[i].nbChargingPoints), sizeof(int), 1, file);

        // Lecture du nombre de places
        fread(&(stations[i].nbAvailableChargingPoints), sizeof(int), 1, file);

        // Lecture de la longitude
        fread(&(stations[i].coord->longitude), sizeof(float), 1, file);

        // Lecture de la latitude
        fread(&(stations[i].coord->latitude), sizeof(float), 1, file);

        stations[i].queues = malloc(stations[i].nbChargingPoints * sizeof(Queue*));

        for (int j = 0; j < stations[i].nbChargingPoints; ++j) {
            stations[i].queues[j] = createQueue();
        }
    }

    // Fermeture du fichier
    fclose(file);

    printf("Stations désérialisées depuis le fichier %s\n", filename);

    return stations;
}

// Fonction qui ajoute une personne à une station
void addPersonToStation(ChargingStation* station, Person* person, int timeOffset,int dist_to_next_station) {

    if (station->nbAvailableChargingPoints != 0) {
        push(station->queues[station->nbChargingPoints - station->nbAvailableChargingPoints], person, timeOffset);
        station->nbAvailableChargingPoints--;
        person->remainingAutonomy += timeOffset * (person->vehicle->fastCharge);
    } else {
        Queue* bestQueue = getBestQueue(station);
        push(bestQueue, person, timeOffset);
    }
    person->chargingTime = timeToFastCharge(person, dist_to_next_station);
}

// Fonction qui retire une personne d'une station
void removePersonFromStation(ChargingStation* station, Person* person) {

    person->chargingTime = 0;

    for (int i = 0; i < station->nbAvailableChargingPoints; ++i) {
        if (first(station->queues[i]) == person) {
            pop(station->queues[i]);
            if(station->queues[i]->data == NULL){
                station->nbAvailableChargingPoints++;
            }
            return;
        }
    }

}

Queue* getBestQueue(ChargingStation* station) {
    Queue* bestQueue = station->queues[0];
    for (int i = 1; i < station->nbChargingPoints - station->nbAvailableChargingPoints; ++i) {
        if (timeToWait(station->queues[i]) < timeToWait(bestQueue)) {
            bestQueue = station->queues[i];
        }
    }
    return bestQueue;
}
