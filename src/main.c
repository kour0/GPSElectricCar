#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures/vehicle.h"
#include "structures/coordinate.h"
#include "structures/charging_station.h"
#include "structures/graph.h"
#include "structures/queue.h"
#include "constants.h"
#include <unistd.h>
#include <time.h>
#include "pthread.h"


int main(int argc, char** argv) {

    // Initialisation du temps d'exécution
    //double time_spent = 0.0;
    //clock_t begin = clock();

    // Lecture du fichier JSON
    int n; // Nombre de stations de recharge
    int m; // Nombre de véhicules

    Vehicle* vehicles;

    // Si le fichier existe dans data, on le deserialize sinon on le crée
    if (access("../data/ev-data.bin", F_OK) == -1) {
        // Création du fichier JSON
        vehicles = readJSONvehicles("../data/ev-data.json", &m);

        // On stocke les stations de recharge
        serializeVehicles("../data/ev-data.bin", vehicles, m);
    } else {
        // On récupère les stations de recharge
        vehicles = deserializeVehicles("../data/ev-data.bin", &m);
    }

    // Affichage du nombre de stations de recharge
    // printf("Nombre de stations de recharge : %d\n", n);
    //printf("Nombre de véhicules : %d\n", m);
    
    // On récupère en argument les coordonnées du point de départ et d'arrivée
    if (argc != 6) {
        printf("Usage : %s <latitude depart> <longitude depart> <latitude arrivee> <longitude arrivee>\n", argv[0]);
        return 1;
    }
    float lat1 = atof(argv[1]);
    float lon1 = atof(argv[2]);
    float lat2 = atof(argv[3]);
    float lon2 = atof(argv[4]);

    // On récupère le véhicule choisi par l'utilisateur en argument qui est le nom du véhicule
    int i = 0;
    if (argc == 6) {
        while (i < m && strcmp(vehicles[i].name, argv[5]) != 0) {
            ++i;
        }
        if (i == m) {
            printf("Véhicule %s non trouvé\n", argv[5]);
            return 1;
        }
    }
    else {
        printf("Veuillez choisir un véhicule parmi la liste suivante :\n");
        for (int j = 0; j < m; ++j) {
            printf("%s\n", vehicles[j].name);
        }
        return 1;
    }

    printf("Véhicule choisi : %s\n", vehicles[i].name);
    printf("Autonomie : %d m\n", vehicles[i].range);
    //printf("Temps de recharge : %d\n", vehicles[i].fastCharge);

    // Ajout des stations de départ et d'arrivée (48.672894, 6.1582773) et (48.6834253, 6.1617406) ou (50.3933812, 3.6062753)
    Coordinate* start = malloc(sizeof(Coordinate));
    start->latitude = lat1;
    start->longitude = lon1;
    Coordinate* end = malloc(sizeof(Coordinate));
    end->latitude = lat2;
    end->longitude = lon2;

    ChargingStation* stations;

    // Si le fichier existe dans data, on le deserialize sinon on le crée
    if (access("../data/data_mod.bin", F_OK) == -1) {
        // Création du fichier JSON
        stations = readJSONstations("../data/data_mod.json", &n);

        // On stocke les stations de recharge
        serializeStations("../data/data_mod.bin", stations, n);
    } else {
        // On récupère les stations de recharge
        stations = deserializeStations("../data/data_mod.bin", &n);
    }

    // Afficher la distance entre le point de départ et d'arrivée
    printf("Distance entre le point de départ et d'arrivée : %d\n", distance(start, end));

    // Affichage des stations de recharge
    // printStations(stations, n);

    Graph* graph;

    // Si le graph n'est pas déjà créer et présent dans le dossier data, on le crée sinon on le récupère
    if (access("../data/graph.bin", F_OK) == -1) {
        // Création du graphe pondéré
        graph = createGraphFromStations(stations, n);

        // On stocke la matrice d'adjacence
        serializeGraph(graph, "../data/graph.bin");
    } else {
        // On récupère la matrice d'adjacence
        graph = deserializeGraph("../data/graph.bin", n);
    }

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court avec un thread
    int* pathLength = malloc(sizeof(int));

    // On crée un thread
    pthread_t thread_id;
    // On crée un tableau de paramètres pour l'algorithme de Dijkstra
    ThreadParamsDijkstra paramsDijkstra;
    paramsDijkstra.graph = graph;
    paramsDijkstra.src = start;
    paramsDijkstra.dest = end;
    paramsDijkstra.stations = stations;
    paramsDijkstra.n = pathLength;
    paramsDijkstra.range = vehicles[i].range;
    paramsDijkstra.autonomy = vehicles[i].range;

    pthread_create(&thread_id, NULL, dijkstra, (void *) &paramsDijkstra);

    // On attend la fin du thread et on récupère le chemin le plus court
    int* res = NULL;
    pthread_join(thread_id, (void**) &res);

    // Affichage du chemin le plus court
    printf("Chemin le plus court est de longueur %d : \n", *pathLength);
    printf("\n");
    printPath(stations, res, *pathLength, start, end);
    // Url base carte maps without api
    char url[1000] = "https://www.google.com/maps/dir/";

    // On ajoute les coordonnées de départ
    char deplat1[20];
    char deplon1[20];
    sprintf(deplat1, "%f", start->latitude);
    sprintf(deplon1, "%f", start->longitude);
    strcat(url, deplat1);
    strcat(url, ",");
    strcat(url, deplon1);
    strcat(url, "/");

    // On ajoute les coordonnées des stations de recharge

    for (int i = 1; i < (*pathLength)-1; ++i) {
        char lat[20];
        char lon[20];
        sprintf(lat, "%f", stations[res[i]].coord->latitude);
        sprintf(lon, "%f", stations[res[i]].coord->longitude);
        strcat(url, lat);
        strcat(url, ",");
        strcat(url, lon);
        strcat(url, "/");
    }

    // On ajoute les coordonnées d'arrivée
    char arrlat2[20];
    char arrlon2[20];
    sprintf(arrlat2, "%f", end->latitude);
    sprintf(arrlon2, "%f", end->longitude);
    strcat(url, arrlat2);
    strcat(url, ",");
    strcat(url, arrlon2);

    // On affiche l'url
    printf("\n");
    printf("Url : %s\n", url);


    // On affiche le chemin le plus court en utilisant le véhicule choisi

    // printf("Chemin le plus court en utilisant le véhicule %s : \n", vehicles[i].name);
    // int* pathLengthVehicle = malloc(sizeof(int));
    // int pourcentageMinRange = 0;
    // int* resVehicle = reducePath(vehicles+i, stations, res, *pathLength, pathLengthVehicle, pourcentageMinRange);

    // Affichage du chemin le plus court

    // printf("Chemin le plus court est de longueur %d : \n", *pathLengthVehicle);
    // printPath(stations, resVehicle, *pathLengthVehicle);

    // Print graph
    // printGraph(graph);

    // Libération de la mémoire
    
    // free(resVehicle);
    // free(pathLengthVehicle);

    free(start);
    free(end);
    free(res);
    free(pathLength);
    freeGraph(graph);
    for (int i = 0; i < n; ++i) {
        free(stations[i].name);
        free(stations[i].coord);
        for (int j = 0; j < stations[i].nbChargingPoints; ++j) {
            free(stations[i].queues[j]);
        }
        free(stations[i].queues);
    }
    for (int i = 0; i < m; ++i) {
        free(vehicles[i].name);
    }
    free(stations);
    free(vehicles);

    // Calcul du temps d'exécution
    //clock_t end_time = clock();
    //time_spent = (double)(end_time - begin) / CLOCKS_PER_SEC;
    //printf("Temps d'exécution : %f\n", time_spent);

    return 0;
}
