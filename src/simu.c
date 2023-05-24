//
// Created by kour0 on 5/12/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures/vehicle.h"
#include "structures/coordinate.h"
#include "structures/charging_station.h"
#include "structures/graph.h"
#include "constants.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    Person* person;
    Coordinate* next_station;
    ChargingStation* stations;
} ThreadParamsSimu;


void* nextStep(void* param) {

    ThreadParamsSimu* params = (ThreadParamsSimu*) param;
    Person* person = params->person;
    Coordinate* next_station = params->next_station;
    ChargingStation* stations = params->stations;

    // On est arrivé à destination
    if (person->pathSize == 1) {
        pthread_exit(NULL);
    }
    // On est sur un chemin
    if (person->chargingTime == 0) {
        int step_dist = STEP * VITESSE;
        int dist_to_next_station = distance(person->coordinate, next_station);
        Coordinate* new_coord = NULL;
        if(dist_to_next_station < step_dist) {
            // On arrive à la station
            if(pos_equals(person->end, next_station)) {
                // On est arrivé
                person->pathSize = 1;
                pthread_exit(NULL);
            }else{
                int timeOffset = (step_dist - dist_to_next_station) / VITESSE;
                int dist_next_station = distance(next_station, stations[person->path[2]].coord);
                new_coord = next_station;
                addPersonToStation(&stations[person->path[1]], person, timeOffset, dist_next_station);
                person->remainingAutonomy -= dist_to_next_station;
            }

        }else {
             new_coord = pos_after_step(person->coordinate, next_station, step_dist);
             person->remainingAutonomy -= step_dist;
        }
        free(person->coordinate);
        person->coordinate = new_coord;  // (3.606275, 50.393383)
    } else {
        // On est dans une station
        if (person->waitingTime > 0) {
            // On attend
            if (person->waitingTime > STEP) {
                person->waitingTime -= STEP;
            } else {
                int timeOffset = STEP - person->waitingTime;
                person->waitingTime = 0;
                person->chargingTime -= timeOffset;
            }
        } else {
            // On charge
            if (person->chargingTime > STEP) {
                person->chargingTime -= STEP;
                person->remainingAutonomy += STEP * person->vehicle->fastCharge;
            } else {
                // On a fini de charger
                int timeOffset = STEP - person->chargingTime;
                person->remainingAutonomy += person->chargingTime * person->vehicle->fastCharge;
                removePersonFromStation(&stations[person->path[1]], person);
                Coordinate* new_coord = pos_after_step(person->coordinate, next_station, timeOffset * VITESSE);
                free(person->coordinate);
                person->coordinate = new_coord;
            }
        }
    }

    pthread_exit(NULL);
}

/*void nextStep_old(Graph* graph, Person* person, Coordinate* next_station, ChargingStation* stations) {
    // On est arrivé à destination
    if (person->pathSize == 1) {
        return;
    }
    // On est sur un chemin
    if (person->remainingTime == 0) {
        Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP, &(person->remainingTime), &person->pathSize);
        person->remainingAutonomy -= distance(person->coordinate, new_coord);
        if (person->end->longitude == new_coord->longitude && person->end->latitude == new_coord->latitude) {
            // On est arrivé
            person->pathSize = 1;
        } else {
            if (next_station->longitude == new_coord->longitude && next_station->latitude == new_coord->latitude) {
                printf("On rentre dans une station\n");
                addPersonToStation(stations, person, person->path[1]);
            }
        }
        free(person->coordinate);
        person->coordinate = new_coord;  // (3.606275, 50.393383)
    } else {
        // On est dans une station
        if (person->remainingTime - STEP >= 0) {
            person->remainingTime -= STEP;
            person->remainingAutonomy += STEP * person->vehicle->fastCharge;
        }
        else {
            // On sort de la station avec une distance en plus
            person->remainingAutonomy += person->remainingTime * person->vehicle->fastCharge;
            Coordinate* new_coord = pos_after_step(person->coordinate, next_station, STEP - person->remainingTime, &(person->remainingTime), &person->pathSize);
            person->remainingTime -= distance(person->coordinate, new_coord);
            free(person->coordinate);
            person->coordinate = new_coord;
            person->remainingTime = 0;
        }
    }
    int* new_path = dijkstra(graph, stations, person->remainingAutonomy, person->vehicle->range, person->coordinate, person->end, &(person->pathSize));
    free(person->path);
    person->path = new_path;
}*/

int main(int argc, char** argv) {

    // Initialisation du temps d'exécution
    double time_spent = 0.0;
    clock_t begin = clock();

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
    printf("Nombre de véhicules : %d\n", m);

    // On récupère le véhicule choisi par l'utilisateur en argument qui est le nom du véhicule
    int i = 0;
    if (argc == 2) {
        while (i < m && strcmp(vehicles[i].name, argv[1]) != 0) {
            ++i;
        }
        if (i == m) {
            printf("Véhicule %s non trouvé\n", argv[1]);
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
    printf("Autonomie : %d\n", vehicles[i].range);
    printf("Temps de recharge : %d\n", vehicles[i].fastCharge);

    // On récupère les coordonnées des points de départs et d'arrivées que l'on recupère dans un fichier texte appelé input.txt où chaque ligne correspond à une lattitude et une longitude
    FILE* file = fopen("../data/input.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return 1;
    }
    int nb_coords = 0;
    while (!feof(file)) {
        float lat, lon;
        fscanf(file, "%f %f\n", &lat, &lon);
        ++nb_coords;
    }
    rewind(file);
    printf("Nombre de coordonnées : %d\n", nb_coords);
    // On stocke les coordonnées dans un tableau
    Coordinate** coords = malloc(nb_coords * sizeof (Coordinate*));
    for (int j = 0; j < nb_coords; ++j) {
        coords[j] = malloc(sizeof (Coordinate));
        fscanf(file, "%f %f\n", &(coords[j]->latitude), &(coords[j]->longitude));
        printf("%f %f\n", coords[j]->latitude, coords[j]->longitude);
    }
    fclose(file);

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

    printf("Nombre de stations de recharge : %d\n", n);

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

    // Afficher la distance entre le point de départ et d'arrivée
    // printf("Distance entre le point de départ et d'arrivée : %d\n", distance(start, end));

    // Affichage des stations de recharge
    // printStations(stations, n);

    // Affichage de la matrice d'adjacence
    // printAdjMat(graph->adjMat, n);

    Person** persons = malloc(nb_coords/2 * sizeof(Person*));

    // On crée les threads
    int numThreads = nb_coords/2;
    pthread_t threadsDijkstraStart[numThreads];
    ThreadParamsDijkstra paramsDijkstraStart[numThreads];

    for (int z=0 ; z<nb_coords ; z+=2) {

        // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
        int *pathLength = malloc(sizeof(int));

        printf("Autonomie test : %d\n", vehicles[i].range);

        paramsDijkstraStart[z / 2].graph = graph;
        paramsDijkstraStart[z / 2].stations = stations;
        paramsDijkstraStart[z / 2].range = vehicles[i].range;
        paramsDijkstraStart[z / 2].autonomy = vehicles[i].range;
        paramsDijkstraStart[z / 2].src = coords[z];
        paramsDijkstraStart[z / 2].dest = coords[z + 1];
        paramsDijkstraStart[z / 2].n = pathLength;

        pthread_create(&threadsDijkstraStart[z/2], NULL, dijkstra, (void *) &paramsDijkstraStart[z/2]);

//        int *res = dijkstra(graph, stations, vehicles[i].range, vehicles[i].range, coords[z], coords[z+1], pathLength);

    }

    for (int z=0 ; z<nb_coords ; z+=2) {

        int* new_path = NULL;
        pthread_join(threadsDijkstraStart[z/2], (void**)&new_path);

        // Affichage du chemin le plus court
        printf("Chemin le plus court est de longueur %d : \n", *paramsDijkstraStart[z / 2].n);
        printPath(stations, new_path, *paramsDijkstraStart[z / 2].n, coords[z], coords[z+1]);

        // On crée la person

        Person *person = createPerson(vehicles + i, coords[z], new_path, *paramsDijkstraStart[z / 2].n, coords[z+1]);

        persons[z/2] = person;

        free(paramsDijkstraStart[z / 2].n);

    }

    // commande à chaque step : person->path = dijkstra(graph, stations, person->vehicle, person->coordinate, stations[person->path[person->pathSize]].coord, &(person->pathSize));

    int step = 0;
    bool finished = false;

    while (!finished) {
        printf("Step %d\n", step); //(3.606275, 50.393383)

        pthread_t threads[numThreads];
        ThreadParamsSimu params[numThreads];

        for (int i = 0; i < numThreads; ++i) {

            params[i].person = persons[i];
            params[i].stations = stations;
            if (persons[i]->pathSize == 2) {
                params[i].next_station = persons[i]->end;
            } else {
                params[i].next_station = stations[persons[i]->path[1]].coord;
            }

            pthread_create(&threads[i], NULL, nextStep, (void *) &params[i]);

        }

        for (int i = 0; i < numThreads; ++i) {
            pthread_join(threads[i], NULL);
        }

        pthread_t threadsDijkstra[numThreads];
        ThreadParamsDijkstra paramsDijkstra[numThreads];

        for (int i = 0; i < numThreads; ++i) {

            if (persons[i]->chargingTime != 0) {

                paramsDijkstra[i].graph = graph;
                paramsDijkstra[i].stations = stations;
                paramsDijkstra[i].autonomy = persons[i]->remainingAutonomy;
                paramsDijkstra[i].range = persons[i]->vehicle->range;
                paramsDijkstra[i].src = persons[i]->coordinate;
                paramsDijkstra[i].dest = persons[i]->end;
                paramsDijkstra[i].n = &(persons[i]->pathSize);

                /*int* new_path = dijkstra(graph, stations, person->remainingAutonomy, person->vehicle->range, person->coordinate, person->end, &(person->pathSize));
                free(person->path);
                person->path = new_path;*/

                pthread_create(&threadsDijkstra[i], NULL, dijkstra, (void *) &paramsDijkstra[i]);

            } else {
                // On crée un thread vide si la personne n'a pas besoin de recharger
                void *(*emptyThread)(void *) = NULL;
                pthread_create(&threadsDijkstra[i], NULL, emptyThread, NULL);
            }
        }

        for (int i = 0; i < numThreads; ++i) {
            int* new_path;
            pthread_join(threadsDijkstra[i], (void**)&new_path);
            if (new_path != NULL) {
                free(persons[i]->path);
                persons[i]->path = new_path;
            }
        }

        //if (person->pathSize == 2) {
        //    nextStep(graph, person, person->end, stations);
        //} else {
        //    nextStep(graph, person, stations[person->path[1]].coord, stations);
        //}
        for (int i = 0; i < numThreads; ++i) {
            if (persons[i]->pathSize == 1) {
                finished = true;
            } else {
                finished = false;
                break;
            }
        }

        // Affichage des informations de la personne
        for (int i = 0; i < numThreads; ++i) {
            printf("Position : %f %f\n", persons[i]->coordinate->latitude, persons[i]->coordinate->longitude);
            printf("Autonomie restante : %d\n", persons[i]->remainingAutonomy);
            printf("Distance restante : %d\n", persons[i]->pathSize);
            printf("Taux de recharge : %d\n", persons[i]->vehicle->fastCharge);
            printf("Temps d'attente : %d\n", persons[i]->waitingTime);
            printf("Temps de recharge : %d\n", persons[i]->remainingAutonomy);
            printf("Chemin : \n");
            printPath(stations, persons[i]->path, persons[i]->pathSize, persons[i]->coordinate, persons[i]->end);

            // Url base carte maps without api
            char url[1000] = "https://www.google.com/maps/dir/";

            // Ajout du chemin de la personne à l'url
            for (int j = 0; j < persons[i]->pathSize; ++j) {
                char coord[50];
                if (j == 0) {
                    sprintf(coord, "%f,%f/", persons[i]->coordinate->latitude, persons[i]->coordinate->longitude);
                } else {
                    if (j == persons[i]->pathSize - 1) {
                        sprintf(coord, "%f,%f", persons[i]->end->latitude, persons[i]->end->longitude);
                    } else {
                        sprintf(coord, "%f,%f/", stations[persons[i]->path[j]].coord->latitude,
                                stations[persons[i]->path[j]].coord->longitude);
                    }
                }
                strcat(url, coord);
            }
            printf("\n");
        }
        ++step;
        // Attendre un input dans la console
        getchar();
    }

    // On libère la mémoire
    for (int k = 0; k < n; ++k) {
        free(stations[k].name);
        free(stations[k].coord);
        free(stations[k].queues);
    }
    for (int j = 0; j < m; ++j) {
        free(vehicles[j].name);
    }
    free(stations);
    free(vehicles);
    freeGraph(graph);
    free(coords);
    for (int l = 0; l < nb_coords/2; ++l) {
        free(persons[i]->coordinate); // (5.531192, 49.150017)
        free(persons[i]->path);
        free(persons[i]->end);
        free(persons[i]);
    }
    free(persons);

    // Calcul du temps d'exécution
    clock_t end_time = clock();
    time_spent = (double)(end_time - begin) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %f\n", time_spent);

    return 0;
}
