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
    if (argc != 5) {
        printf("Usage : %s <latitude depart> <longitude depart> <latitude arrivee> <longitude arrivee>\n", argv[0]);
        return 1;
    }
    float lat1 = atof(argv[1]);
    float lon1 = atof(argv[2]);
    float lat2 = atof(argv[3]);
    float lon2 = atof(argv[4]);

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

    // Affichage des stations de recharge
    // printStations(stations, n);

    
    // Création du graphe pondéré
    struct Graph* graph = createGraphFromStations(stations, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Sommet source et sommet destination pour le chemin le plus court
    int src = n-2;
    int dest = n-1;

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
    int* pathLength = malloc(sizeof(int));
    int* res = dijkstra(graph, src, dest, pathLength);

    // Affichage du chemin le plus court
    printf("Chemin le plus court est de longueur %d : ", *pathLength);
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
    free(stations);

    return 0;
}
