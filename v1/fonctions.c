#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "cJSON.h"
#include "fonctions.h"

// Fonction pour initialiser un graphe pondéré à l'aide d'une liste contigue à une dimension
Graph* createGraph(int V) {

    long V_double = (long) V;

    // Allocation de la mémoire pour le graphe
    printf("Allocation de la mémoire pour le graphe de taille : %ld\n", ((V_double * (V_double+1))/2-V_double));

    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adjMat = calloc(((V_double * (V_double+1))/2-V_double), sizeof(int));
    return graph;
}

// Fonction pour calculer la distance entre deux coordonnées géographiques en km
int distance(Coordinate coord1, Coordinate coord2) {
    // Conversion des coordonnées en radians
    double lat1 = coord1.latitude * M_PI / 180;
    double lon1 = coord1.longitude * M_PI / 180;
    double lat2 = coord2.latitude * M_PI / 180;
    double lon2 = coord2.longitude * M_PI / 180;

    // Calcul de la distance
    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return (int) (EARTH_RADIUS * c);
}

// Fonction pour lire le fichier JSON
ChargingStation* readJSON(char* filename, int* n) {
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
    cJSON* features = cJSON_GetObjectItemCaseSensitive(json, "stations");
    *n = cJSON_GetArraySize(features);

    // Allocation du tableau de stations
    ChargingStation* stations = malloc(*n * sizeof(ChargingStation));

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        cJSON* feature = cJSON_GetArrayItem(features, i);
        cJSON* name = cJSON_GetObjectItemCaseSensitive(feature, "nom_station");
        cJSON* longitude = cJSON_GetObjectItemCaseSensitive(feature, "longitude");
        cJSON* latitude = cJSON_GetObjectItemCaseSensitive(feature, "latitude");

        stations[i].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(stations[i].name, name->valuestring);
        stations[i].coord.longitude = longitude->valuedouble;
        stations[i].coord.latitude = latitude->valuedouble;
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    return stations;
}

// Fonction pour créer le graphe pondéré
Graph* createGraphFromStations(ChargingStation* stations, int n) {
    Graph* graph = createGraph(n);

    // Calcul des distances entre les stations
    for (int i = 0; i < n-1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            graph->adjMat[i*(n-1)-((i-1)*i)/2+j-(i+1)] = distance(stations[i].coord, stations[j].coord);
        }
    }

    return graph;
}

// Fonction pour libérer la mémoire
void freeGraph(Graph* graph) {
    free(graph->adjMat);
    free(graph);
}

// Fonction pour afficher le graphe
void printGraph(Graph* graph) {
    for (int i = 0; i < graph->V; ++i) {
        for (int j = 0; j < graph->V; ++j) {
            if (i == j) {
                printf("0 ");
            } else if (i < j) {
                printf("%d ", graph->adjMat[i*(graph->V-1) - ((i-1)*i)/2 + j - (i+1)]);
            } else {
                printf("%d ", graph->adjMat[j*(graph->V-1) - ((j-1)*j)/2 + i - (j+1)]);
            }
        }
        printf("\n");
    }
}

// Algorithme de Dijkstra pour trouver le plus court chemin entre deux stations
int* dijkstra(Graph* graph, int src, int dest, int* n) {
    // Initialisation des tableaux
    int* dist = malloc(graph->V * sizeof(int));
    int* prev = malloc(graph->V * sizeof(int));
    bool* visited = calloc(graph->V, sizeof(bool));

    // Initialisation des distances
    for (int i = 0; i < graph->V; ++i) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    dist[src] = 0;

    // Boucle principale
    for (int i = 0; i < graph->V; ++i) {
        // Recherche du sommet non visité le plus proche
        int min = INT_MAX;
        int u = -1;
        for (int j = 0; j < graph->V; ++j) {
            if (!visited[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }

        // Si on a pas trouvé de sommet, on arrête
        if (u == -1) {
            break;
        }

        // On marque le sommet comme visité
        visited[u] = true;

        // On met à jour les distances
        for (int v = 0; v < graph->V; ++v) {
            if (!visited[v]) {
                int w = 0;
                if (u < v) {
                    w = graph->adjMat[u*(graph->V-1) - ((u-1)*u)/2 + v - (u+1)];
                } else {
                    w = graph->adjMat[v*(graph->V-1) - ((v-1)*v)/2 + u - (v+1)];
                }
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                }
            }
        }
    }

    // On récupère le chemin
    int* path = malloc(graph->V * sizeof(int));
    int pathLength = 0;
    int u = dest;
    while (u != -1) {
        path[pathLength++] = u;
        u = prev[u];
    }

    // On libère la mémoire
    free(dist);
    free(prev);
    free(visited);

    // On retourne le chemin
    int* result = malloc(pathLength * sizeof(int));
    for (int i = 0; i < pathLength; ++i) {
        result[i] = path[pathLength - i - 1];
    }
    free(path);
    *n = pathLength;
    return result;
}

void printPath(ChargingStation* stations, int* path, int n) {
    for (int i = 0; i < n; ++i) {
        printf("%s (%f, %f) -> ", stations[path[i]].name, stations[path[i]].coord.longitude, stations[path[i]].coord.latitude);
    }
    printf("FIN\n");
}
