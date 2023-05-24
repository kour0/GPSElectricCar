
#include <stdio.h>
#include <stdlib.h>
#include "test_utils.h"
#include "../src/structures/charging_station.h"


int main(void) {

    ChargingStation *station = malloc(sizeof(ChargingStation));
    station->name = "Station 1";
    station->nbChargingPoints = 2;
    station->nbAvailableChargingPoints = 2;

    station->queues = malloc(sizeof(Queue*) * station->nbChargingPoints);
    for (int i = 0; i < station->nbChargingPoints; i++) {
        station->queues[i] = createQueue();
    }

    Vehicle *vehicle1 = malloc(sizeof(Vehicle));
    vehicle1->fastCharge = 100;
    vehicle1->range = 100000;

    Person *person1 = createPerson(vehicle1, NULL, NULL, 0, NULL);
    person1->remainingAutonomy = 0;

    Person *person2 = createPerson(vehicle1, NULL, NULL, 0, NULL);
    person2->remainingAutonomy = 0;

    Person *person3 = createPerson(vehicle1, NULL, NULL, 0, NULL);
    person3->remainingAutonomy = 0;

    Person *person4 = createPerson(vehicle1, NULL, NULL, 0, NULL);
    person4->remainingAutonomy = 0;

    addPersonToStation(station, person1, 0, 30000);

    assert_true(station->nbAvailableChargingPoints == 1, "La station doit avoir une place de moins disponible");

    assert_true(station->queues[0]->data == person1, "La personne doit être dans la première file");

    assert_true(person1->waitingTime == 0, "La personne ne doit pas attendre");

    assert_true(person1->chargingTime == 300, "La personne doit charger pendant 300 secondes");

    removePersonFromStation(station, person1);

    assert_true(station->nbAvailableChargingPoints == 2, "La station doit avoir une place de plus disponible");

    assert_true(station->queues[0]->data == NULL, "La première file doit être vide");

    assert_true(person1->chargingTime == 0, "La personne ne doit plus charger");

    person1->remainingAutonomy = 10000;
    addPersonToStation(station, person1, 0, 30000);

    assert_true(person1->chargingTime == 200, "Il reste 10 km d'autonomie, la personne doit charger pendant 100 secondes");

    addPersonToStation(station, person2, 0, 10000);

    assert_true(station->nbAvailableChargingPoints == 0, "La station doit être pleine");

    assert_true(station->queues[1]->data == person2, "La personne doit être dans la deuxième file");

    assert_true(person2->waitingTime == 0, "La personne ne doit pas attendre");

    assert_true(person2->chargingTime == 100, "La personne doit charger pendant 100 secondes");

    addPersonToStation(station, person3, 0, 40000);

    assert_true(station->nbAvailableChargingPoints == 0, "La station doit être pleine");

    assert_true(station->queues[1]->next->data == person3, "La personne doit être dans la deuxième file");

    assert_true(person3->waitingTime == 100, "La personne doit attendre 100 secondes");

    assert_true(person3->chargingTime == 400, "La personne doit charger pendant 100 secondes");

    addPersonToStation(station, person4, 0, 20000);

    assert_true(station->nbAvailableChargingPoints == 0, "La station doit être pleine");

    assert_true(station->queues[0]->next->data == person4, "La personne doit être dans la première file");

    assert_true(person4->waitingTime == 200, "La personne doit attendre 200 secondes");



}
