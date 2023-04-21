#include <stdbool.h>

// Constantes
#define M_PI 3.14159265358979323846 // Valeur de pi
#define EARTH_RADIUS 6378.137 // Rayon de la Terre en km
#define FLOAT_MAX 3.402823466e+38F // Valeur maximale d'un float

// Structure pour stocker les coordonnées géographiques
typedef struct Coordinate {
    float latitude; // Latitude
    float longitude; // Longitude
} Coordinate;

// Structure pour stocker une station de recharge
typedef struct ChargingStation {
    char* name; // Nom de la station
    Coordinate coord; // Coordonnées géographiques
} ChargingStation;

// Structure pour stocker les véhicules
typedef struct Vehicle {
    char* name; // Nom du véhicule
    int fastCharge; // Capacité de charge rapide
    int range; // Autonomie
} Vehicle;

// Structure de graphe pondéré
typedef struct Graph {
    int V; // Nombre de sommets
    double* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
double distance(Coordinate coord1, Coordinate coord2);
ChargingStation* readJSONstations(char* filename, int* n, ChargingStation* depart, ChargingStation* arrivee);
Vehicle* readJSONvehicles(char* filename, int* n);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
int* dijkstra(Graph* graph, Vehicle* vehicle, int src, int dest, int* n);
void printPath(ChargingStation* stations, int* path, int n);
int* reducePath(Vehicle* vehicle, ChargingStation* stations, int* path, int n, int* nReduced, int pourcentageMinRange);
