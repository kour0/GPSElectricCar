//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_GRAPH_H
#define PPII2_GRP_30_GRAPH_H
#include "charging_station.h"
#include "vehicle.h"
#include "coordinate.h"

typedef struct Graph {
    int V; // Nombre de sommets
    double* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
int* dijkstra(Graph* graph, ChargingStation* stations, Vehicle* vehicle, Coordinate* src, Coordinate* dest, int* n);
void printPath(ChargingStation* stations, int* path, int n);
void serializeGraph(Graph* graph, char* filename);
Graph* deserializeGraph(char* filename, int V);

#endif //PPII2_GRP_30_GRAPH_H
