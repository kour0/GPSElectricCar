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

    // Allocation de la mémoire pour le graphe
    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adj = malloc(V*sizeof(List));
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
Graph* createGraphFromStations(ChargingStation* stations, Vehicle* vehicle, int n) {
    Graph* graph = createGraph(n);

    // Calcul des distances entre les stations

    // Largeur de la barre de progression
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
    for (int i = 0; i < n; ++i) {
        graph->adj[i].data = malloc(n * sizeof(Node));
        int size = 0;
        for (int j = i + 1; j < n; ++j) {
            // Si la distance est inférieur à la range du véhicule, alors il est accessible
            float dist = distance(stations[i].coord, stations[j].coord);
            if (dist <= vehicle->range) {
                graph->adj[i].data[size].point = j;
                graph->adj[i].data[size].distance = dist;
                size++;
            }
        }
        graph->adj[i].data = realloc(graph->adj[i].data, size * sizeof(Node));
        graph->adj[i].size = size;
        // On affiche une progress bar
        // printf("\rProgression : %d%%", (int) ((i+1) * 100 / n));
        // fflush(stdout);
        progress = i*100.0/(double)MAX;
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
    for (int i = 0; i < graph->V; ++i) {
        free(graph->adj[i].data);
    }
    free(graph->adj);
    free(graph);
}

int getMinDistance(float* dist, bool* visited, int V) {
    int minIndex = -1;
    float minValue = INFINITY;
    for (int i = 0; i < V; ++i) {
        if (!visited[i] && dist[i] < minValue) {
            minIndex = i;
            minValue = dist[i];
        }
    }
    return minIndex;
}

// Algorithme de Dijkstra pour trouver le plus court chemin entre deux stations
int* dijkstra(Graph* graph, int src, int dest, int* n) {
    // On initialise les tableaux
    float* dist = malloc(graph->V * sizeof(float));
    int* prev = malloc(graph->V * sizeof(int));
    bool* visited = malloc(graph->V * sizeof(bool));

    // On initialise les distances à l'infini
    for (int i = 0; i < graph->V; ++i) {
        dist[i] = INFINITY;
        prev[i] = -1;
        visited[i] = false;
    }

    // On initialise la distance de la source à 0
    dist[src] = 0;

    // On parcourt tous les sommets
    for (int i = 0; i < graph->V; ++i) {
        // On récupère le sommet le plus proche
        int u = getMinDistance(dist, visited, graph->V);

        // Si on a pas trouvé de sommet, alors on arrête
        if (u == -1) {
            break;
        }

        // Si on a trouvé le sommet de destination, alors on arrête
        if (u == dest) {
            break;
        }

        // On marque le sommet comme visité
        visited[u] = true;

        // On met à jour les distances en sachant que si u > v alors on doit aller dans la liste d'adjacence de v
        List adj;
        if (u > dest) {
            adj = graph->adj[dest];
        } else {
            adj = graph->adj[u];
        }
        for (int j = 0; j < adj.size; ++j) {
            int v = adj.data[j].point;
            float distance = adj.data[j].distance;
            if (!visited[v] && dist[u] + distance < dist[v]) {
                dist[v] = dist[u] + distance;
                prev[v] = u;
            }
        }
    }

    // On récupère le chemin
    printf("Récupération du chemin...\n");
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
