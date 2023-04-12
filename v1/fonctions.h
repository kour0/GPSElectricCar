#include <stdbool.h>

// Constantes
#define M_PI 3.14159265358979323846 // Valeur de pi

// Structure pour stocker les coordonnées géographiques
typedef struct Coordinate {
    double latitude; // Latitude
    double longitude; // Longitude
} Coordinate;

// Structure pour stocker une station de recharge
typedef struct ChargingStation {
    char* name; // Nom de la station
    Coordinate coord; // Coordonnées géographiques
} ChargingStation;

// Structure de graphe pondéré
typedef struct Graph {
    int V; // Nombre de sommets
    int* adjMat; // Matrice d'adjacence de taille V*(V+1)/2-V et de dimension 1
} Graph; // Structure de graphe pondéré à l'aide d'une liste contigue à une dimension

Graph* createGraph(int V);
int distance(Coordinate coord1, Coordinate coord2);
ChargingStation* readJSON(char* filename, int* n);
Graph* createGraphFromStations(ChargingStation* stations, int n);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
