//
// Created by kour0 on 5/10/23.
//

#ifndef PPII2_GRP_30_PERSON_H
#define PPII2_GRP_30_PERSON_H

typedef struct Person {
    Vehicle* vehicle;
    Coordinate* coordinate;
    int* path;
    ChargingStation* chargingStation;
    float remainingTime;
    float autonomy;
};

float timeToFastCharge(Person* person, int distance);

#endif //PPII2_GRP_30_PERSON_H
