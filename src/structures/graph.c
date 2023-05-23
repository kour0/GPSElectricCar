//
// Created by Noe Steiner on 22/04/2023.
//

#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../constants.h"
#include <time.h>
#include <stdbool.h>

Graph* createGraph(int V) {

    long V_double = (long) V;

    // Allocation de la mémoire pour le graphe
    printf("Allocation de la mémoire pour le graphe de taille : %ld\n", ((V_double * (V_double+1))/2-V_double));

    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adjMat = calloc(((V_double * (V_double+1))/2-V_double), sizeof(double));
    return graph;
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
int* dijkstra(Graph* graph, ChargingStation* stations, int autonomy, int range, Coordinate* src, Coordinate* dest, int* n) {

    // Initialisation temps
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // Si le départ est la destination
    if (src->longitude == dest->longitude && src->latitude == dest->latitude) {
        *n = 1;
        int* path = malloc(sizeof(int));
        path[0] = graph->V;

        // Fin temps
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Temps dijsktra : %f\n", cpu_time_used);

        return path;
    }

    // Initialisation des tableaux
    int* dist = malloc((graph->V+2) * sizeof(int));
    int* prev = malloc((graph->V+2) * sizeof(int));
    bool* visited = calloc((graph->V+2), sizeof(bool));

    // Initialisation des distances
    for (int i = 0; i < graph->V+2; ++i) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    dist[graph->V] = 0;

    // Boucle principale
    for (int i = 0; i < graph->V+2; ++i) {

        // Recherche du sommet non visité le plus proche
        int min = INT_MAX;
        int u = -1;
        for (int j = 0; j < graph->V+2; ++j) {
            if (!visited[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }

        // Si on a pas trouvé de sommet, on arrête
        if (u == -1) {
            printf("Pas de chemin trouvé\n");
            break;
        }

        // Si on a trouvé le sommet de destination, on arrête
        if (u == graph->V+1) {
            break;
        }

        // On marque le sommet comme visité
        visited[u] = true;

        // On met à jour les distances
        for (int v = 0; v < graph->V+2; ++v) {
            if (!visited[v]) {
                int w = 0;
                if (v == graph->V) {
                    w=distance(src, stations[u].coord);
                } else {
                    if (v == graph->V+1) {
                        if (u == graph->V) {
                            w=distance(src, dest);
                        } else {
                            w=distance(stations[u].coord, dest);
                        }
                    } else {
                        if (u == graph->V) {
                            w = distance(src, stations[v].coord);
                        } else {
                            if (u < v) {
                                w = graph->adjMat[u * (graph->V - 1) - ((u - 1) * u) / 2 + v - (u + 1)];
                            } else {
                                w = graph->adjMat[v * (graph->V - 1) - ((v - 1) * v) / 2 + u - (v + 1)];
                            }
                        }
                    }
                }
                if (w >= autonomy) {
                    continue;
                }
                int new_dist = dist[u] + w;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    prev[v] = u;
                }
            }
        }
        autonomy = range;
    }

    // On récupère le chemin
    int* path = malloc(graph->V * sizeof(int));
    int pathLength = 0;
    int u = graph->V+1;
    if (prev[u] == -1) {
        printf("Pas de chemin trouvé\n");
        return NULL;
    }
    path[pathLength++] = u;
    u = prev[u];
    while (u != -1) {
        if (u == graph->V) {
            path[pathLength++] = u;
            u = prev[u];
            continue;
        }
        if (stations[u].coord->longitude == src->longitude && stations[u].coord->latitude == src->latitude) {
            u = graph->V;
            path[pathLength++] = u;
            u = prev[u];
        } else {
            path[pathLength++] = u;
            u = prev[u];
        }
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

    // Fin temps
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps dijsktra : %f\n", cpu_time_used);
    printf("Liste dijkstra : ");
    for (int i = 0; i < pathLength; ++i) {
        printf("%d ", result[i]);
    }
    return result;
}

// Fonction pour afficher le chemin
void printPath(ChargingStation* stations, int* path, int n, Coordinate* src, Coordinate* dest) {

    int totalDistance = 0;
    if (n== 1) {
        printf("Arrivée\n");
        return;
    }
    printf("DEBUT\n");
    if (n == 2) {
        printf("Début (%f, %f) -> (distance : %d) ", src->longitude, src->latitude, distance(src, dest));
        printf("%s (%f, %f)\n", "Fin", dest->longitude, dest->latitude);
        printf("Distance totale : %d km\n", distance(src, dest));
        printf("FIN\n");
        return;
    }
    printf("Début (%f, %f) -> (distance : %d) ", src->longitude, src->latitude, distance(src, stations[path[1]].coord));
    totalDistance += distance(src, stations[path[1]].coord);
    for (int i = 1; i < n-2; ++i) {
        printf("%s (%f, %f) -> (distance : %d) ", stations[path[i]].name, stations[path[i]].coord->longitude, stations[path[i]].coord->latitude, distance(stations[path[i]].coord, stations[path[i+1]].coord));
        totalDistance += distance(stations[path[i]].coord, stations[path[i+1]].coord);
    }
    printf("%s (%f, %f) -> (distance : %d) ", stations[path[n-2]].name, stations[path[n-2]].coord->longitude, stations[path[n-2]].coord->latitude, distance(stations[path[n-2]].coord, dest));
    totalDistance += distance(stations[path[n-2]].coord, dest);
    printf("%s (%f, %f)\n", "Fin", dest->longitude, dest->latitude);
    printf("Distance totale : %d km\n", totalDistance);
    printf("FIN\n");
}

// Fonction pour stocker la matrice d'adjacence en binaire
void serializeGraph(Graph* graph, char* filename) {
    FILE* file = fopen(filename, "wb");
    fwrite(graph->adjMat, sizeof(int), graph->V*(graph->V-1)/2, file);
    fclose(file);
}

// Fonction pour récupérer la matrice d'adjacence en binaire dont on connait la taille
Graph* deserializeGraph(char* filename, int V) {
    FILE* file = fopen(filename, "rb");
    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adjMat = malloc(graph->V*(graph->V-1)/2 * sizeof(int));
    fseek(file, 0, SEEK_SET);
    fread(graph->adjMat, sizeof(int), graph->V*(graph->V-1)/2, file);
    fclose(file);
    return graph;
}
