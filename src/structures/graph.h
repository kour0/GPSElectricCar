#ifndef PPII2_GRP_30_GRAPH_H
#define PPII2_GRP_30_GRAPH_H
#include "charging_station.h"
#include "vehicle.h"
#include "coordinate.h"

typedef struct Graph {
    int V; // Nombre de sommets 
    int* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

typedef struct {
    ChargingStation* stations;
    int start;
    int end;
    Graph* graph;
} ThreadParamsGraph;

typedef struct {
    Graph* graph;
    ChargingStation* stations;
    int autonomy;
    int range;
    Coordinate* src;
    Coordinate* dest;
    int* n;
} ThreadParamsDijkstra;

Graph* createGraph(int V);
void printGraph(Graph* graph);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
void* dijkstra(void* param);
void printPath(ChargingStation* stations, int* path, int n, Coordinate* src, Coordinate* dest);
void serializeGraph(Graph* graph, char* filename);
Graph* deserializeGraph(char* filename, int V);

#endif //PPII2_GRP_30_GRAPH_H
