//
// Created by Bozo Steiner on 22/04/2017.
//

#ifndef PPII2_GRP_30_GRAPH_H
#define PPII2_GRP_30_GRAPH_H
#include "charging_station.h"
#include "vehicle.h"
#include "coordinate.h"

typedef struct Graph {
    int V; // Nombre de sommets nik sa mere celui qui a mis V pour un nombre de sommet
    int* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
void printGraph(Graph* graph);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
int* dijkstra(Graph* graph, ChargingStation* stations, int autonomy, int range, Coordinate* src, Coordinate* dest, int* n);
void printPath(ChargingStation* stations, int* path, int n, Coordinate* src, Coordinate* dest);
void serializeGraph(Graph* graph, char* filename);
Graph* deserializeGraph(char* filename, int V);

#endif //PPII2_GRP_30_GRAPH_H
