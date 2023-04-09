#include <stdbool.h>

// Constantes
#define M_PI 3.14159265358979323846 // Valeur de pi

// Structure pour stocker les coordonnées géographiques
struct Coordinate {
    double latitude; // Latitude
    double longitude; // Longitude
};

// Structure pour stocker une station de recharge
struct ChargingStation {
    char* name; // Nom de la station
    struct Coordinate coord; // Coordonnées géographiques
};

// Structure de graphe pondéré
struct Graph {
    int V; // Nombre de sommets
    double** adjMat; // Matrice d'adjacence
};

struct Graph* createGraph(int V);
double distance(struct Coordinate coord1, struct Coordinate coord2);
struct ChargingStation* readJSON(char* filename, int* n);
void printStations(struct ChargingStation* stations, int n);
void printAdjMat(double** adjMat, int n);
struct Graph* createGraphFromStations(struct ChargingStation* stations, int n);
void freeGraph(struct Graph* graph);
int minDistance(double* dist, bool* sptSet, int V);
void dijkstra(struct Graph* graph, int src, int dest);
