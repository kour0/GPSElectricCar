#include <stdbool.h>

// Constantes
#define M_PI 3.14159265358979323846 // Valeur de pi
#define EARTH_RADIUS 6371 // Rayon de la Terre en km
#define INT_MAX 2147483647 // Valeur maximale d'un entier

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
    int* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
int distance(Coordinate coord1, Coordinate coord2);
ChargingStation* readJSONstations(char* filename, int* n);
Vehicle* readJSONvehicles(char* filename, int* n);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
int* dijkstra(Graph* graph, int src, int dest, int* n);
void printPath(ChargingStation* stations, int* path, int n);
