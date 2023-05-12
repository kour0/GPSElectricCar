//
// Created by Noe Steiner on 22/04/2023.
//

#ifndef PPII2_GRP_30_GRAPH_H
#define PPII2_GRP_30_GRAPH_H
#include "charging_station.h"
#include "vehicle.h"

typedef struct Graph {
    int V; // Nombre de sommets
    double* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
int* dijkstra(Graph* graph, Vehicle* vehicle, int src, int dest, int* n);
void printPath(ChargingStation* stations, int* path, int n);
int* reducePath(Vehicle* vehicle, ChargingStation* stations, int* path, int n, int* nReduced, int pourcentageMinRange);
void serializeGraph(Graph* graph, char* filename);
Graph* deserializeGraph(char* filename, int V);
int sizePath(int* path);

#endif //PPII2_GRP_30_GRAPH_H
