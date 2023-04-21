#include <stdbool.h>

// Constantes
#define M_PI 3.14159265358979323846 // Valeur de pi
#define DIST_MAX 10000 // Valeur maximale de la distance entre deux points
#define EARTH_RADIUS 6378.137 // Rayon de la Terre en km

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

typedef struct Node {
    int distance; // distance
    int point; // point
} Node;

typedef struct List {
    Node* data; // Tableau de données
    int size; // Taille du tableau
} List;

// Structure de graphe pondéré
typedef struct Graph {
    int V; // Nombre de sommets
    List* adj; // Liste d'adjacence (tableau de listes)
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
double distance(Coordinate coord1, Coordinate coord2);
ChargingStation* readJSONstations(char* filename, int* n);
Vehicle* readJSONvehicles(char* filename, int* n);
Graph* createGraphFromStations(ChargingStation* stations, Vehicle* vehicle, int n);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
int* dijkstra(Graph* graph, int src, int dest, int* n);
void printPath(ChargingStation* stations, int* path, int n);
int* reducePath(Vehicle* vehicle, ChargingStation* stations, int* path, int n, int* nReduced, int pourcentageMinRange);
