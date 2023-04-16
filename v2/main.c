#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "fonctions.h"

int main(int argc, char** argv) {
    // Lecture du fichier JSON
    int n; // Nombre de stations de recharge
    int m; // Nombre de véhicules
    ChargingStation* stations = readJSONstations("../data/data_mod.json", &n);
    Vehicle* vehicles = readJSONvehicles("../data/ev-data.json", &m);

    // Affichage du nombre de stations de recharge
    printf("Nombre de stations de recharge : %d\n", n);
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
    start.coord.latitude = lat1;
    start.coord.longitude = lon1;
    struct ChargingStation end;
    end.name = malloc(8 * sizeof(char));
    strcpy(end.name, "Arrivee");
    end.coord.latitude = lat2;
    end.coord.longitude = lon2;
    stations = realloc(stations, (n + 2) * sizeof(struct ChargingStation));
    stations[n] = start;
    stations[n + 1] = end;
    n += 2;

    // Afficher la distance entre le point de départ et d'arrivée
    printf("Distance entre le point de départ et d'arrivée : %f\n", distance(start.coord, end.coord));

    // Affichage des stations de recharge
    // printStations(stations, n);

    
    // Création du graphe pondéré
    struct Graph* graph = createGraphFromStations(stations, vehicles+i, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Sommet source et sommet destination pour le chemin le plus court
    int src = n-2;
    int dest = n-1;

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
    int* pathLength = malloc(sizeof(int));
    printf("Appel de l'algorithme de Dijkstra...\n");
    int* res = dijkstra(graph, src, dest, pathLength);

    // Affichage du chemin le plus court
    printf("Chemin le plus court est de longueur %d : \n", *pathLength);
    printPath(stations, res, *pathLength);

    // Print graph
    // printGraph(graph);

    // Libération de la mémoire
    free(res);
    free(pathLength);
    freeGraph(graph);
    for (int i = 0; i < n; ++i) {
        free(stations[i].name);
    }
    for (int i = 0; i < m; ++i) {
        free(vehicles[i].name);
    }
    free(stations);
    free(vehicles);

    return 0;
}
