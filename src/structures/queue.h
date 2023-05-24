//
// Created by kour0 on 5/10/23.
//

#ifndef PPII2_GRP_30_QUEUE_H
#define PPII2_GRP_30_QUEUE_H

#include "person.h"

typedef struct Queue {
    Person* data;
    struct Queue* next;
    struct Queue* prev;
} Queue;

Queue* createQueue(void);
void del_person(Queue* queue);
void push(Queue* queue, Person* data, int timeOffset);
Person* last(Queue* queue);
Person* index_of_from(Queue* queue, int index);
int timeToWait(Queue* queue);
Person* first(Queue* queue);
void pop(Queue* queue);


#endif //PPII2_GRP_30_QUEUE_H
