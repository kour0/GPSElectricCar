#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "fonctions.h"

int main() {
    // Lecture du fichier JSON
    int n; // Nombre de stations de recharge
    struct ChargingStation* stations = readJSON("../data/data_mod.json", &n);

    // Affichage du nombre de stations de recharge
    printf("Nombre de stations de recharge : %d\n", n);
    
    // On récupère en argument les coordonnées du point de départ et d'arrivée
    // if (argc != 5) {
    //     printf("Usage : %s <latitude depart> <longitude depart> <latitude arrivee> <longitude arrivee>\n", argv[0]);
    //     return 1;
    // }
    // double lat1 = atof(argv[1]);
    // double lon1 = atof(argv[2]);
    // double lat2 = atof(argv[3]);
    // double lon2 = atof(argv[4]);

    // Ajout des stations de départ et d'arrivée
    // struct ChargingStation start;
    // start.name = "Depart";
    // start.coord.latitude = lat1;
    // start.coord.longitude = lon1;
    // struct ChargingStation end;
    // end.name = "Arrivee";
    // end.coord.latitude = lat2;
    // end.coord.longitude = lon2;
    // stations = realloc(stations, (n + 2) * sizeof(struct ChargingStation));
    // stations[n] = start;
    // stations[n + 1] = end;
    // n += 2;

    // Affichage des stations de recharge
    // printStations(stations, n);

    
    // Création du graphe pondéré
    struct Graph* graph = createGraphFromStations(stations, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Sommet source et sommet destination pour le chemin le plus court
    // int src = n-2;
    // int dest = n-1;

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
    // dijkstra(graph, src, dest);

    // Print graph
    // printGraph(graph);

    // Libération de la mémoire
    freeGraph(graph);
    for (int i = 0; i < n; ++i) {
        free(stations[i].name);
    }
    free(stations);

    return 0;
}
