#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include "cJSON.h"

#define MAX_STATIONS 1000
#define MAX_VERTICES 1000
#define M_PI 3.14159265358979323846

// Structure pour stocker les coordonnées géographiques
struct Coordinate {
    double latitude;
    double longitude;
};

// Structure pour stocker une station de recharge
struct ChargingStation {
    char* name;
    struct Coordinate coord;
};

// Structure de graphe pondéré
struct Graph {
    int V; // Nombre de sommets
    double** adjMat; // Matrice d'adjacence
};

// Fonction pour initialiser un graphe pondéré
struct Graph* createGraph(int V) {
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->V = V;
    graph->adjMat = malloc(V * sizeof(double*));
    for (int i = 0; i < V; ++i) {
        graph->adjMat[i] = calloc(V, sizeof(double));
    }
    return graph;
}

// Fonction pour calculer la distance entre deux coordonnées géographiques
double distance(struct Coordinate coord1, struct Coordinate coord2) {
    double lat1 = coord1.latitude * M_PI / 180;
    double lat2 = coord2.latitude * M_PI / 180;
    double lon1 = coord1.longitude * M_PI / 180;
    double lon2 = coord2.longitude * M_PI / 180;
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return 6371 * c;
}

// Fonction pour lire le fichier JSON
struct ChargingStation* readJSON(char* filename, int* n) {
    // Ouverture du fichier
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Lecture du fichier
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* string = malloc(fsize + 1);
    fread(string, 1, fsize, file);
    fclose(file);
    string[fsize] = 0;

    // Parsing du fichier JSON du type:
    // {
//   "type": "FeatureCollection",
//   "features": [
//     {
//       "type": "Feature",
//       "geometry": {
//         "type": "Point",
//         "coordinates": [
//           3.1485441,
//           50.7582304
//         ]
//       },
//       "properties": {
//         "nom_amenageur": "LUMI'IN",
//         "siren_amenageur": "807940069"
//       }
//     },
//     ...
//   ]
    cJSON* json = cJSON_Parse(string);
    free(string);
    if (json == NULL) {
        printf("Erreur lors du parsing du fichier JSON\n");
        exit(1);
    }

    // Récupération du nombre de stations
    cJSON* features = cJSON_GetObjectItemCaseSensitive(json, "features");
    *n = cJSON_GetArraySize(features);

    // Allocation du tableau de stations
    struct ChargingStation* stations = malloc(*n * sizeof(struct ChargingStation));

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        cJSON* feature = cJSON_GetArrayItem(features, i);
        cJSON* geometry = cJSON_GetObjectItemCaseSensitive(feature, "geometry");
        cJSON* coordinates = cJSON_GetObjectItemCaseSensitive(geometry, "coordinates");
        cJSON* properties = cJSON_GetObjectItemCaseSensitive(feature, "properties");
        cJSON* name = cJSON_GetObjectItemCaseSensitive(properties, "nom_amenageur");

        stations[i].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(stations[i].name, name->valuestring);
        stations[i].coord.longitude = cJSON_GetArrayItem(coordinates, 0)->valuedouble;
        stations[i].coord.latitude = cJSON_GetArrayItem(coordinates, 1)->valuedouble;
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    return stations;
}

// Fonction pour afficher les stations de recharge
void printStations(struct ChargingStation* stations, int n) {
    for (int i = 0; i < n; ++i) {
        printf("%s (%lf, %lf)\n", stations[i].name, stations[i].coord.latitude, stations[i].coord.longitude);
    }
}

// Fonction pour afficher la matrice d'adjacence
void printAdjMat(double** adjMat, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%lf ", adjMat[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour créer le graphe pondéré
struct Graph* createGraphFromStations(struct ChargingStation* stations, int n) {
    struct Graph* graph = createGraph(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            graph->adjMat[i][j] = distance(stations[i].coord, stations[j].coord);
        }
    }
    return graph;
}

// Fonction pour libérer la mémoire
void freeGraph(struct Graph* graph) {
    for (int i = 0; i < graph->V; ++i) {
        free(graph->adjMat[i]);
    }
    free(graph->adjMat);
    free(graph);
}

// Fonction pour trouver l'indice du sommet avec la distance minimale dans l'ensemble des sommets non encore inclus dans le chemin le plus court
int minDistance(double* dist, bool* sptSet, int V) {
    double min = DBL_MAX;
    int minIndex = -1;
    for (int v = 0; v < V; ++v) {
        if (sptSet[v] == false && dist[v] <= min) {
            min = dist[v];
            minIndex = v;
        }
    }
    return minIndex;
}

// Fonction pour implémenter l'algorithme de Dijkstra pour trouver le chemin le plus court entre deux sommets d'un graphe pondéré
void dijkstra(struct Graph* graph, int src, int dest) {
    int V = graph->V;
    double* dist = malloc(V * sizeof(double)); // Tableau pour stocker les distances minimales
    int* parent = malloc(V * sizeof(int)); // Tableau pour stocker les parents des sommets dans le chemin le plus court

    bool* sptSet = malloc(V * sizeof(bool)); // Tableau pour stocker les sommets inclus dans le chemin le plus court

    // Initialiser toutes les distances à l'infini et marquer tous les sommets comme non inclus dans le chemin le plus court
    for (int i = 0; i < V; ++i) {
        dist[i] = DBL_MAX;
        sptSet[i] = false;
    }

    // La distance du sommet source à lui-même est toujours 0
    dist[src] = 0;
    parent[src] = -1; // Le sommet source n'a pas de parent

    // Trouver le chemin le plus court pour tous les sommets
    for (int count = 0; count < V - 1; ++count) {
        int u = minDistance(dist, sptSet, V); // Choisir le sommet avec la distance minimale non encore inclus dans le chemin le plus court
        sptSet[u] = true; // Marquer le sommet comme inclus dans le chemin le plus court

        // Mettre à jour les distances des sommets adjacents du sommet choisi
        for (int v = 0; v < V; ++v) {
            if (!sptSet[v] && graph->adjMat[u][v] && dist[u] != DBL_MAX && dist[u] + graph->adjMat[u][v] < dist[v]) {
                dist[v] = dist[u] + graph->adjMat[u][v];
                parent[v] = u;
            }
        }
    }

    // Afficher le chemin le plus court et sa distance
    printf("Chemin le plus court de la station %d à la station %d : \n", src, dest);
    int currentNode = dest;
    printf("%d ", currentNode);
    while (parent[currentNode] != -1) {
        printf("<- %d ", parent[currentNode]);
        currentNode = parent[currentNode];
    }
    printf("\n");
    printf("Distance : %lf km\n", dist[dest]);

    // Libérer la mémoire
    free(dist);
    free(parent);
    free(sptSet);
}

int main() {
    // Lecture du fichier JSON
    int n;
    struct ChargingStation* stations = readJSON("data.json", &n);

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

    // Libération de la mémoire
    freeGraph(graph);
    for (int i = 0; i < n-2; ++i) {
        free(stations[i].name);
    }
    free(stations);

    return 0;
}
