#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "cJSON.h"
#include "fonctions.h"
#define WIDTH 100

// Fonction pour initialiser un graphe pondéré à l'aide d'une liste contigue à une dimension
Graph* createGraph(int V) {

    long V_double = (long) V;

    // Allocation de la mémoire pour le graphe
    printf("Allocation de la mémoire pour le graphe de taille : %ld\n", ((V_double * (V_double+1))/2-V_double));

    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adjMat = calloc(((V_double * (V_double+1))/2-V_double), sizeof(float));
    return graph;
}

// Fonction pour calculer la distance entre deux coordonnées géographiques en km
float distance(Coordinate coord1, Coordinate coord2) {
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
    return (EARTH_RADIUS * c);
}

// Fonction pour lire le fichier JSON et récupérer les stations de recharge
ChargingStation* readJSONstations(char* filename, int* n) {
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

// Fonction pour lire le fichier JSON et récupérer les véhicules
Vehicle* readJSONvehicles(char* filename, int* n) {
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
    
    cJSON* json = cJSON_Parse(string);
    free(string);
    if (json == NULL) {
        printf("Erreur lors du parsing du fichier JSON\n");
        exit(1);
    }

    // Récupération du nombre de stations
    cJSON* vehicles = cJSON_GetObjectItemCaseSensitive(json, "Vehicles");
    *n = cJSON_GetArraySize(vehicles);

    // Allocation du tableau de stations
    Vehicle* tab_vehicles = malloc(*n * sizeof(Vehicle));

    // Récupération des stations
    for (int i = 0; i < *n; ++i) {
        cJSON* vehicle = cJSON_GetArrayItem(vehicles, i);
        cJSON* name = cJSON_GetObjectItemCaseSensitive(vehicle, "Name");
        cJSON* range = cJSON_GetObjectItemCaseSensitive(vehicle, "Range");
        cJSON* fastCharge = cJSON_GetObjectItemCaseSensitive(vehicle, "Fast Charge");

        tab_vehicles[i].name = malloc((strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(tab_vehicles[i].name, name->valuestring);
        tab_vehicles[i].range = atoi(range->valuestring);
        tab_vehicles[i].fastCharge = atoi(fastCharge->valuestring);
    }

    // Libération de la mémoire
    cJSON_Delete(json);

    return tab_vehicles;
}

// Fonction pour créer le graphe pondéré
Graph* createGraphFromStations(ChargingStation* stations, int n) {
    Graph* graph = createGraph(n);

    // Calcul des distances entre les stations
    float progress = 0.0;
    int c  = 0, x=0, last_c=0;
    int MAX = n - 1;
    /**
     * Print a basic template of the progress line.
     **/
    fprintf(stderr, "%3d%% [", (int)progress);
    for (x = 0; x < c; x++){
        fprintf(stderr, "=");
    }
    for (x = c; x < WIDTH; x++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "]");
    for (int i = 0; i < n-1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            graph->adjMat[i*(n-1)-((i-1)*i)/2+j-(i+1)] = distance(stations[i].coord, stations[j].coord);
        }
        // On affiche une progress bar
        // printf("\rProgression : %d%%", (int) ((i+1) * 100 / n));
        // fflush(stdout);
        progress = i*100.0/MAX+1;
        c = (int) progress;
        /**
         * Update the template on each increment in progress.
        **/
        fprintf(stderr, "\n\033[F");
        fprintf(stderr, "%3d%%", (int)progress);
        fprintf(stderr, "\033[1C");
        fprintf(stderr, "\033[%dC=", last_c);
        for (x = last_c; x < c; x++){
        fprintf(stderr, "=");
        }
        if(x<WIDTH){
        fprintf(stderr, ">");
        }
        last_c = c;
    }
    // printf("\rProgression : 100%%\n");
    /**
    * Write a finish line.
     **/
    fprintf(stderr, "\033[EDone\n");

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
    float* dist = malloc(graph->V * sizeof(float));
    int* prev = malloc(graph->V * sizeof(int));
    bool* visited = calloc(graph->V, sizeof(bool));

    // Initialisation des distances
    for (int i = 0; i < graph->V; ++i) {
        dist[i] = FLOAT_MAX;
        prev[i] = -1;
    }
    dist[src] = 0;

    // Boucle principale
    for (int i = 0; i < graph->V; ++i) {
        // Recherche du sommet non visité le plus proche
        float min = FLOAT_MAX;
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
                float w = 0;
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

// Fonction qui réduit la taille du chemin en supprimant les stations inutiles où la voiture n'a pas besoin de recharger
int* reducePath(Vehicle* vehicle, ChargingStation* stations, int* path, int n, int* nReduced) {
    // On crée un nouveau tableau pour le chemin réduit
    int* reducedPath = malloc(n * sizeof(int));
    int reducedPathLength = 0;
    float remainingRange = vehicle->range;

    // On ajoute la première station
    reducedPath[reducedPathLength++] = path[0];

    // On parcourt le chemin
    for (int i = 0; i < n-1; ++i) {
        // On calcule la distance entre la station actuelle et la station suivante
        float dist = distance(stations[path[i]].coord, stations[path[i+1]].coord);

        // Si la distance est supérieure à la portée de la voiture, on ajoute la station actuelle au chemin réduit
        if (dist > remainingRange) {
            reducedPath[reducedPathLength++] = path[i];
            remainingRange = vehicle->range;
            if (dist > remainingRange) {
                printf("Impossible de recharger la voiture entre les stations %s et %s)", stations[path[i]].name, stations[path[i+1]].name);
                exit(1);
            }
            remainingRange -= dist;
        } else {
            remainingRange -= dist;
        }
    }

    // On ajoute la dernière station (qui est l'arrivée)
    reducedPath[reducedPathLength++] = path[n-1];

    // On libère la mémoire
    int* result = malloc(reducedPathLength * sizeof(int));
    for (int i = 0; i < reducedPathLength; ++i) {
        result[i] = reducedPath[i];
    }
    free(reducedPath);
    *nReduced = reducedPathLength;
    return result;
}

// Fonction pour afficher le chemin
void printPath(ChargingStation* stations, int* path, int n) {
    float totalDistance = 0;
    for (int i = 0; i < n-1; ++i) {
        printf("%s (%f, %f) -> (distance : %f) ", stations[path[i]].name, stations[path[i]].coord.longitude, stations[path[i]].coord.latitude, distance(stations[path[i]].coord, stations[path[i+1]].coord));
        totalDistance += distance(stations[path[i]].coord, stations[path[i+1]].coord);
    }
    printf("%s (%f, %f)\n", stations[path[n-1]].name, stations[path[n-1]].coord.longitude, stations[path[n-1]].coord.latitude);
    printf("Distance totale : %f km\n", totalDistance);
    printf("FIN\n");
}
