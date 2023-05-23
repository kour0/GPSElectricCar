//
// Created by kour0 on 5/12/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures/vehicle.h"
#include "structures/coordinate.h"
#include "structures/charging_station.h"
#include "structures/graph.h"
#include "constants.h"
#include <unistd.h>
#include <time.h>

void nextStep(Person* person, Coordinate* next_station, ChargingStation* stations) {
    // On est arrivé à destination
    if (person->pathSize == 1) {
        return;
    }
    // On est sur un chemin
    if (person->remainingTime == 0) {
        Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP, &(person->remainingTime), &person->pathSize);
        person->autonomy -= distance(person->coordinate, new_coord);
        if (person->end->longitude == new_coord->longitude && person->end->latitude == new_coord->latitude) {
            // On est arrivé
            person->pathSize = 1;
        } else {
            if (next_station->longitude == new_coord->longitude && next_station->latitude == new_coord->latitude) {
                printf("On rentre dans une station\n");
                addPersonToStation(stations, person, person->path[1]);
            }
        }
        free(person->coordinate);
        person->coordinate = new_coord;  // (3.606275, 50.393383)
    } else {
        // On est dans une station
        if (person->remainingTime - STEP >= 0) {
            person->remainingTime -= STEP;
            person->autonomy += STEP/3600.0 * person->vehicle->fastCharge;
        }
        else {
            // On sort de la station avec une distance en plus
            person->autonomy += person->remainingTime/3600.0 * person->vehicle->fastCharge;
            Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP - person->remainingTime, &(person->remainingTime), &person->pathSize);
            person->autonomy -= distance(person->coordinate, new_coord);
            free(person->coordinate);
            person->coordinate = new_coord;
            person->remainingTime = 0;
        }
    }
}

int main(int argc, char** argv) {

    // Initialisation du temps d'exécution
    double time_spent = 0.0;
    clock_t begin = clock();

    // Lecture du fichier JSON
    int n; // Nombre de stations de recharge
    int m; // Nombre de véhicules

    Vehicle* vehicles;

    // Si le fichier existe dans data, on le deserialize sinon on le crée
    if (access("../data/ev-data.bin", F_OK) == -1) {
        // Création du fichier JSON
        vehicles = readJSONvehicles("../data/ev-data.json", &m);

        // On stocke les stations de recharge
        serializeVehicles("../data/ev-data.bin", vehicles, m);
    } else {
        // On récupère les stations de recharge
        vehicles = deserializeVehicles("../data/ev-data.bin", &m);
    }

    // Affichage du nombre de stations de recharge
    // printf("Nombre de stations de recharge : %d\n", n);
    printf("Nombre de véhicules : %d\n", m);

    // On récupère en argument les coordonnées du point de départ et d'arrivée
    if (argc != 6) {
        printf("Usage : %s <latitude depart> <longitude depart> <latitude arrivee> <longitude arrivee>\n", argv[0]);
        return 1;
    }
    float lat1 = atof(argv[1]);
    float lon1 = atof(argv[2]);
    float lat2 = atof(argv[3]);
    float lon2 = atof(argv[4]);

    // On récupère le véhicule choisi par l'utilisateur en argument qui est le nom du véhicule
    int i = 0;
    if (argc == 6) {
        while (i < m && strcmp(vehicles[i].name, argv[5]) != 0) {
            ++i;
        }
        if (i == m) {
            printf("Véhicule %s non trouvé\n", argv[5]);
            return 1;
        }
    }
    else {
        printf("Veuillez choisir un véhicule parmi la liste suivante :\n");
        for (int j = 0; j < m; ++j) {
            printf("%s\n", vehicles[j].name);
        }
        return 1;
    }

    printf("Véhicule choisi : %s\n", vehicles[i].name);
    printf("Autonomie : %f\n", vehicles[i].range);
    printf("Temps de recharge : %d\n", vehicles[i].fastCharge);

    // On crée les coordonnées du point de départ et d'arrivée
    Coordinate* start = malloc(sizeof (Coordinate));
    start->latitude = lat1;
    start->longitude = lon1;
    Coordinate* end = malloc(sizeof (Coordinate));
    end->latitude = lat2;
    end->longitude = lon2;

    ChargingStation* stations;

    // Si le fichier existe dans data, on le deserialize sinon on le crée
    if (access("../data/data_mod.bin", F_OK) == -1) {
        // Création du fichier JSON
        stations = readJSONstations("../data/data_mod.json", &n);

        // On stocke les stations de recharge
        serializeStations("../data/data_mod.bin", stations, n);
    } else {
        // On récupère les stations de recharge
        stations = deserializeStations("../data/data_mod.bin", &n);
    }

    printf("Nombre de stations de recharge : %d\n", n);

    Graph* graph;

    // Si le graph n'est pas déjà créer et présent dans le dossier data, on le crée sinon on le récupère
    if (access("../data/graph.bin", F_OK) == -1) {
        // Création du graphe pondéré
        graph = createGraphFromStations(stations, n);

        // On stocke la matrice d'adjacence
        serializeGraph(graph, "../data/graph.bin");
    } else {
        // On récupère la matrice d'adjacence
        graph = deserializeGraph("../data/graph.bin", n);
    }



    // Afficher la distance entre le point de départ et d'arrivée
    printf("Distance entre le point de départ et d'arrivée : %f\n", distance(start, end));

    // Affichage des stations de recharge
    // printStations(stations, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
    int* pathLength = malloc(sizeof(int));
    int* res = dijkstra(graph, stations, vehicles[i].range, vehicles[i].range,start, end, pathLength);

    // Affichage du chemin le plus court
    printf("Chemin le plus court est de longueur %d : \n", *pathLength);
    printPath(stations, res, *pathLength, start, end);

    // On crée la person

    Person* person = createPerson(vehicles+i, start, res, *pathLength, end);

    // Fork pour lancer le simulateur
    // commande à chaque step : person->path = dijkstra(graph, stations, person->vehicle, person->coordinate, stations[person->path[person->pathSize]].coord, &(person->pathSize));

    int step = 0;

    while (person->pathSize != 1) {
        printf("Step %d\n", step); //(3.606275, 50.393383)
        if (person->pathSize == 2) {
            nextStep(person, person->end, stations);
            free(person->path);
            person->path = dijkstra(graph, stations, person->autonomy, person->vehicle->range, person->coordinate, person->end, &(person->pathSize));
        } else {
            nextStep(person, stations[person->path[1]].coord, stations);
            int* new_path = dijkstra(graph, stations, person->autonomy, person->vehicle->range, person->coordinate, person->end, &(person->pathSize));
            free(person->path);
            person->path = new_path;
        }
        printf("Position : %f %f\n", person->coordinate->latitude, person->coordinate->longitude);
        printf("Autonomie : %f\n", person->autonomy);
        printf("Distance restante : %d\n", person->pathSize);
        printf("Temps de recharge : %d\n", person->vehicle->fastCharge);
        printf("Temps restant : %f\n", person->remainingTime);
        printPath(stations, person->path, person->pathSize, person->coordinate, person->end);
        ++step;
        // Attendre un input dans la console
        getchar();
    }

    // On libère la mémoire
    free(pathLength);
    for (int k = 0; k < n; ++k) {
        free(stations[k].name);
        free(stations[k].coord);
        free(stations[k].queue);
    }
    for (int j = 0; j < m; ++j) {
        free(vehicles[j].name);
    }
    free(stations);
    free(vehicles);
    freeGraph(graph);
    free(person->coordinate); // (5.531192, 49.150017)
    free(person->path);
    free(person->end);
    free(person);

    // Calcul du temps d'exécution
    clock_t end_time = clock();
    time_spent = (double)(end_time - begin) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %f\n", time_spent);

    return 0;
}
