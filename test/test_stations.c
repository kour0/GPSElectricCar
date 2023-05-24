
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

    addPersonToStation(station, person1, 100, 30000);

    assert_true(station->nbAvailableChargingPoints == 1, "La station doit avoir une place de moins disponible");

    assert_true(person1->waitingTime == 0, "La personne ne doit pas attendre");

    assert_true(person1->chargingTime == 200, "La personne doit charger pendant 200 secondes");


}
