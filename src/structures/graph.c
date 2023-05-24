#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../constants.h"
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

// Fonction pour calculer la distance entre deux points en m
void* createGraphFromStationsThread(void* param) {
    ThreadParamsGraph * params = (ThreadParamsGraph *)param;
    ChargingStation* stations = params->stations;
    int start = params->start;
    int end = params->end;
    Graph* graph = params->graph;

    for (int i = start; i < end; ++i) {
        for (int j = i + 1; j < graph->V; ++j) {
            graph->adjMat[i*(graph->V-1)-((i-1)*i)/2+j-(i+1)] = distance(stations[i].coord, stations[j].coord) / VITESSE;
        }
    }

    pthread_exit(NULL);
}


Graph* createGraph(int V) {

    long V_long = (long) V;

    // Allocation de la mémoire pour le graphe
    //printf("Allocation de la mémoire pour le graphe de taille : %ld\n", ((V_long * (V_long+1))/2-V_long));

    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->adjMat = calloc((((V_long * (V_long+1))/2)-V_long), sizeof(int));
    return graph;
}

// Fonction pour créer le graphe pondéré
Graph* createGraphFromStations(ChargingStation* stations, int n) {
    Graph* graph = createGraph(n);

    // Initialisation temps pour mesurer le temps d'exécution
    //clock_t start, end;
    //double cpu_time_used;
    //start = clock();

    int numThreads = 16;  // Nombre de threads à utiliser
    pthread_t threads[numThreads];
    ThreadParamsGraph params[numThreads];

    int chunkSize = n / numThreads;
    int remainder = n % numThreads;
    int startIndex = 0;

    for (int i = 0; i < numThreads; ++i) {

        int endIndex = startIndex + chunkSize;

        if (remainder > 0) {
            endIndex++;
            remainder--;
        }

        params[i].stations = stations;
        params[i].start = startIndex;
        params[i].end = endIndex;
        params[i].graph = graph;

        pthread_create(&threads[i], NULL, createGraphFromStationsThread, (void*)&params[i]);

        startIndex = endIndex;
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Fin du temps d'exécution
    //end = clock();
    //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    //printf("Temps d'exécution construction graph avec %d threads : %f\n", numThreads, cpu_time_used);

    return graph;
}


Graph* createGraphFromStations_old(ChargingStation* stations, int n) {
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
void* dijkstra(void* param) {

    // Récupération des paramètres
    ThreadParamsDijkstra* params = (ThreadParamsDijkstra*) param;
    Graph* graph = params->graph;
    Coordinate* src = params->src;
    Coordinate* dest = params->dest;
    int* n = params->n;
    ChargingStation* stations = params->stations;
    int autonomy = params->autonomy;
    int range = params->range;

    // Si le départ est la destination
    if ((src->longitude == dest->longitude && src->latitude == dest->latitude) || *n == 1) {
        *n = 1;
        int* path = malloc(2*sizeof(int));
        path[0] = graph->V;
        path[1] = graph->V+1;

        pthread_exit(path);
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
                    w=distance(src, stations[u].coord) / VITESSE;
                } else {
                    if (v == graph->V+1) {
                        if (u == graph->V) {
                            w=distance(src, dest) / VITESSE;
                        } else {
                            w=distance(stations[u].coord, dest) / VITESSE;
                        }
                    } else {
                        if (u == graph->V) {
                            w = distance(src, stations[v].coord) / VITESSE + timeToWait(getBestQueue(stations+v));
                        } else {
                            if (u < v) {
                                w = graph->adjMat[u * (graph->V - 1) - ((u - 1) * u) / 2 + v - (u + 1)] + timeToWait(getBestQueue(stations+v));
                            } else {
                                w = graph->adjMat[v * (graph->V - 1) - ((v - 1) * v) / 2 + u - (v + 1)] + timeToWait(getBestQueue(stations+v));
                            }
                        }
                    }
                }
                if (w >= autonomy/VITESSE) {
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

    pthread_exit(result);

}

// Fonction pour afficher le chemin
void printPath(ChargingStation* stations, int* path, int n, Coordinate* src, Coordinate* dest) {
    int totalDistance = 0;

    if (n== 1) {
        printf("Arrivée\n");
        return;
    }
    if (n == 2) {
        printf("Début (lon %f, lat %f) -> (distance : %d m) ", src->longitude, src->latitude, distance(src, dest));
        printf("%s (lon %f, lat %f)\n", "Fin", dest->longitude, dest->latitude);
        printf("Distance totale : %d m\n", distance(src, dest));
        return;
    }

    printf("Début (lon %f, lat %f) -> (distance : %d m) ", src->longitude, src->latitude, distance(src, stations[path[1]].coord));
    totalDistance += distance(src, stations[path[1]].coord);
    for (int i = 1; i < n-2; ++i) {
        printf("%s (lon %f, lat %f) -> (distance : %d m) ", stations[path[i]].name, stations[path[i]].coord->longitude, stations[path[i]].coord->latitude, distance(stations[path[i]].coord, stations[path[i+1]].coord));
        totalDistance += distance(stations[path[i]].coord, stations[path[i+1]].coord);
    }
    printf("%s (lon %f, lat %f) -> (distance : %d m) ", stations[path[n-2]].name, stations[path[n-2]].coord->longitude, stations[path[n-2]].coord->latitude, distance(stations[path[n-2]].coord, dest));
    totalDistance += distance(stations[path[n-2]].coord, dest);
    printf("%s (lon %f, lat %f)\n", "Fin", dest->longitude, dest->latitude);
    printf("Distance totale : %d m\n", totalDistance);

}

// Fonction pour stocker la matrice d'adjacence en binaire
void serializeGraph(Graph* graph, char* filename) {
    FILE* file = fopen(filename, "wb");
    long V_long = (long) graph->V;
    fwrite(graph->adjMat, sizeof(int), (((V_long * (V_long+1))/2)-V_long), file);
    fclose(file);
}

// Fonction pour récupérer la matrice d'adjacence en binaire dont on connait la taille
Graph* deserializeGraph(char* filename, int V) {
    FILE* file = fopen(filename, "rb");
    Graph* graph = malloc(sizeof(Graph));
    graph->V = V;
    long V_long = (long) V;
    graph->adjMat = calloc((((V_long * (V_long+1))/2)-V_long), sizeof(int));
    fseek(file, 0, SEEK_SET);
    fread(graph->adjMat, sizeof(int), (((V_long * (V_long+1))/2)-V_long), file);
    fclose(file);
    return graph;
}
