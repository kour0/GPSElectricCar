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


int main(int argc, char** argv) {

    // Initialisation du temps d'exécution
    double time_spent = 0.0;
    clock_t begin = clock();

    // Lecture du fichier JSON
    int n; // Nombre de stations de recharge
    int m; // Nombre de véhicules

    Vehicle* vehicles = readJSONvehicles("../data/ev-data.json", &m);

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
    printf("Autonomie : %d\n", vehicles[i].range);
    printf("Temps de recharge : %d\n", vehicles[i].fastCharge);

    // Ajout des stations de départ et d'arrivée (48.672894, 6.1582773) et (48.6834253, 6.1617406) ou (50.3933812, 3.6062753)
    struct ChargingStation start;
    start.name = malloc(7 * sizeof(char));
    strcpy(start.name, "Depart");
    start.coord= malloc(sizeof(struct Coordinate));
    start.coord->latitude = lat1;
    start.coord->longitude = lon1;
    struct ChargingStation end;
    end.name = malloc(8 * sizeof(char));
    strcpy(end.name, "Arrivee");
    end.coord= malloc(sizeof(struct Coordinate));
    end.coord->latitude = lat2;
    end.coord->longitude = lon2;

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


    stations = realloc(stations, (n + 2) * sizeof(struct ChargingStation));
    stations[n] = start;
    stations[n + 1] = end;
    n += 2;

    // Afficher la distance entre le point de départ et d'arrivée
    printf("Distance entre le point de départ et d'arrivée : %f\n", distance(start.coord, end.coord));

    // Affichage des stations de recharge
    // printStations(stations, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
    int* pathLength = malloc(sizeof(int));
    int* res = dijkstra(graph, stations, vehicles+i, start.coord, end.coord, pathLength);

    // Fork pour lancer le simulateur
    // commande à chaque step : person->path = dijkstra(graph, stations, person->vehicle, person->coordinate, stations[person->path[person->pathSize]].coord, &(person->pathSize));



    // Affichage du chemin le plus court
    printf("Chemin le plus court est de longueur %d : \n", *pathLength);
    printPath(stations, res, *pathLength);

    // On teste le simulateur



    // On libère la mémoire
    free(res);
    free(pathLength);
    for (int k = 0; k < n-2; ++k) {
        free(stations[k].name);
        free(stations[k].coord);
        free(stations[k].queue);
    }
    free(stations[graph->V+1].name);
    free(stations[graph->V+1].coord);
    free(stations[graph->V].name);
    free(stations[graph->V].coord);
    for (int j = 0; j < m; ++j) {
        free(vehicles[j].name);
    }
    free(stations);
    free(vehicles);
    freeGraph(graph);

    // Calcul du temps d'exécution
    clock_t end_time = clock();
    time_spent = (double)(end_time - begin) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %f\n", time_spent);

    return 0;
}

