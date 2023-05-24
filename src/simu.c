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

void* emptyThread(void* arg) {
    // On supprime le warning
    (void) arg;
    return NULL;
}


void nextStep(Person* person, Coordinate* next_station, ChargingStation* stations) {
    // On est arrivé à destination
    if (person->pathSize == 1) {
        return;
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
                return;
            }else{
                int timeOffset = (step_dist - dist_to_next_station) / VITESSE;
                Coordinate* nextNextStation;
                if(person->pathSize == 3) {
                    nextNextStation = person->end;
                }else{
                    nextNextStation = stations[person->path[2]].coord;
                }
                int dist_nextNext_station = distance(next_station, nextNextStation);
                new_coord = malloc(sizeof(Coordinate));
                new_coord->longitude = next_station->longitude;
                new_coord->latitude = next_station->latitude;
                person->remainingAutonomy -= dist_to_next_station;
                addPersonToStation(&stations[person->path[1]], person, timeOffset, dist_nextNext_station);
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
                person->remainingAutonomy += timeOffset * person->vehicle->fastCharge;
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
                Coordinate* new_coord = NULL;
                if(person->pathSize == 3) {
                    new_coord = pos_after_step(person->coordinate, person->end, timeOffset * VITESSE);

                }
                else {
                    new_coord = pos_after_step(person->coordinate, stations[person->path[2]].coord, timeOffset * VITESSE);
                }
                free(person->coordinate);
                person->coordinate = new_coord;
            }
        }
    }

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

/*Person* generateRandomPersonList(ChargingStation* stations, Vehicle* vehicles,int nb_stations, int nb_vehicles,int  nbPerson) {

    Person* personList = NULL;
    for (int i = 0; i < nbPerson; i++) {
        Person* person = generateRandomPerson(stations, vehicles, nb_stations, nb_vehicles);
        person->next = personList;
        personList = person;
    }
}

Person* generateRandomPerson(ChargingStation* stations, Vehicle* vehicles,int nb_stations, int nb_vehicles) {

    Vehicle vehicle = vehicles[rand() % nb_vehicles];
    ChargingStation start = stations[rand() % nb_stations];
    ChargingStation end = stations[rand() % nb_stations];

    return createPerson()
}*/

int main() {

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

    // On récupère les coordonnées des points de départs et d'arrivées que l'on recupère dans un fichier texte appelé input.txt où chaque ligne correspond à une lattitude et une longitude
    FILE* file = fopen("../data/input.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return 1;
    }
    int nb_persons = 0;
    while (!feof(file)) {
        char c = fgetc(file);
        if (c == '\n') {
            nb_persons++;
        }
    }
    nb_persons++;
    rewind(file);
    printf("Nombre de personnes : %d\n", nb_persons);
    // On stocke les coordonnées dans un tableau
    Coordinate** coords = malloc(nb_persons * 2 * sizeof (Coordinate*));
    char **vehicules_input = malloc(nb_persons * sizeof(char*));
    for (int j = 0; j < nb_persons*2; j += 2) {
        coords[j] = malloc(sizeof (Coordinate));
        coords[j+1] = malloc(sizeof (Coordinate));
        char *line = malloc(255 * sizeof (char));
        fscanf(file, "%s %f %f %f %f\n", line, &(coords[j]->latitude), &(coords[j]->longitude), &(coords[j+1]->latitude), &(coords[j+1]->longitude));
        // On supprime les tirets "_" dans le nom des véhicules
        for (int i = 0; i < (int)strlen(line); ++i) {
            if (line[i] == '_') {
                line[i] = ' ';
            }
        }
        vehicules_input[j/2] = line;
        printf("%s %f %f %f %f\n", line, coords[j]->latitude, coords[j]->longitude, coords[j+1]->latitude, coords[j+1]->longitude);
    }
    
    fclose(file);

    // On récupère les véhicules des personnes qui sont dans line
    Vehicle** vehicles_persons = malloc(nb_persons * sizeof (Vehicle*));
    for (int i = 0; i < nb_persons; ++i) {
        for (int j = 0; j < m; ++j) {
            if (strcmp(vehicules_input[i], vehicles[j].name) == 0) {
                vehicles_persons[i] = &(vehicles[j]);
            }
        }
    }

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

    //printf("Nombre de stations de recharge : %d\n", n);

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

    Person** persons = malloc(nb_persons * sizeof(Person*));

    // On crée les threads
    int numThreads = nb_persons;
    pthread_t threadsDijkstraStart[numThreads];
    ThreadParamsDijkstra paramsDijkstraStart[numThreads];

    for (int z=0 ; z<nb_persons*2 ; z+=2) {

        // Appel de l'algorithme de Dijkstra pour trouver le chemin le plus court
        int *pathLength = malloc(sizeof(int));

        paramsDijkstraStart[z / 2].graph = graph;
        paramsDijkstraStart[z / 2].stations = stations;
        paramsDijkstraStart[z / 2].range = vehicles_persons[z/2]->range;
        paramsDijkstraStart[z / 2].autonomy = vehicles_persons[z/2]->range;
        paramsDijkstraStart[z / 2].src = coords[z];
        paramsDijkstraStart[z / 2].dest = coords[z + 1];
        paramsDijkstraStart[z / 2].n = pathLength;

        pthread_create(&threadsDijkstraStart[z/2], NULL, dijkstra, (void *) &paramsDijkstraStart[z/2]);

//        int *res = dijkstra(graph, stations, vehicles[i].range, vehicles[i].range, coords[z], coords[z+1], pathLength);

    }

    for (int z=0 ; z<nb_persons*2 ; z+=2) {

        int* new_path = NULL;
        pthread_join(threadsDijkstraStart[z/2], (void**)&new_path);

        // On crée la person

        Person *person = createPerson(vehicles_persons[z/2], coords[z], new_path, *paramsDijkstraStart[z / 2].n, coords[z+1]);

        persons[z/2] = person;

        free(paramsDijkstraStart[z / 2].n);

    }

    // commande à chaque step : person->path = dijkstra(graph, stations, person->vehicle, person->coordinate, stations[person->path[person->pathSize]].coord, &(person->pathSize));

    int step = 0;
    bool finished = false;

    printf("\n");

    while (!finished) {
        printf("Step %d\n", step); //(3.606275, 50.393383)

        /*pthread_t threads[numThreads];
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
        }*/

        for (int l = 0; l < numThreads; ++l) {

            if (persons[l]->pathSize <= 2) {
                nextStep(persons[l], persons[l]->end, stations);
            } else {
                nextStep(persons[l], stations[persons[l]->path[1]].coord, stations);
            }

        }

        for (int i = 0; i < numThreads; ++i) {
            if (persons[i]->pathSize == 1) {
                finished = true;
            } else {
                finished = false;
                break;
            }
        }

        // Attendre un input dans la console

        pthread_t threadsDijkstra[numThreads];
        ThreadParamsDijkstra paramsDijkstra[numThreads];

        for (int i = 0; i < numThreads; ++i) {

            if (persons[i]->chargingTime ==0) {

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


        // Affichage des informations de la personne
        for (int i = 0; i < numThreads; ++i) {
            
            // On regarde si la personne est dans une station
            bool inStation = false;
            if (persons[i]->chargingTime > 0) {
                inStation = true;
            } else {
                inStation = false;
            }
            if (persons[i]->pathSize != 1) {
                printf("Personne %d\n", i);
                printf("Véhicule : %s\n", persons[i]->vehicle->name);
                if (inStation) {
                    printf("Dans une station : Oui\n");
                } else {
                    printf("Dans une station : Non\n");
                }
                printf("Position : %f %f\n", persons[i]->coordinate->latitude, persons[i]->coordinate->longitude);
                printf("Autonomie restante : %d m\n", persons[i]->remainingAutonomy);
                printf("Etapes restantes : %d\n", persons[i]->pathSize);
                if (persons[i]->chargingTime > 0) {
                    printf("Temps de recharge restant : %d s\n", persons[i]->chargingTime);
                }
                if (persons[i]->waitingTime > 0) {
                    printf("Temps d'attente restant : %d s\n", persons[i]->waitingTime);
                }
                printf("Chemin : \n");
                printf("\n");
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
                printf("Url : %s\n", url);
                printf("\n");
                // On ouvre l'url dans le navigateur par défaut du système d'exploitation avec la commande "open"
                //char command[1000] = "start ";
                //strcat(command, url);
                //system(command);
                //printf("\n");
                } else {
                printf("Personne %d est arrivée à destination\n", i);
                printf("\n");
            }
        }
        ++step;
        // Attendre un input dans la console
        //getchar();
    }

    // On libère la mémoire
    for (int k = 0; k < n; ++k) {
        free(stations[k].name);
        free(stations[k].coord);
        for (int i = 0; i < stations[k].nbChargingPoints; ++i) {
            //free(stations[k].queues[i]->next);
            free(stations[k].queues[i]);
        }
        free(stations[k].queues);
    }
    for (int j = 0; j < m; ++j) {
        free(vehicles[j].name);
    }
    free(stations);
    free(vehicles);
    freeGraph(graph);
    free(coords);
    for (int l = 0; l < nb_persons; ++l) {
        free(persons[l]->coordinate); // (5.531192, 49.150017)
        free(persons[l]->path);
        free(persons[l]->end);
        free(persons[l]);
    }
    free(persons);
    free(vehicles_persons);

    return 0;
}
