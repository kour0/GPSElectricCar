//
// Created by kour0 on 5/10/23.
//

#ifndef PPII2_GRP_30_QUEUE_H
#define PPII2_GRP_30_QUEUE_H

typedef struct Queue {
    Person* data;
    Queue* next;
    Queue* prev;
} Queue;

Queue* create_queue(Person* data);
void del_person(Queue* queue);
void push(Queue* queue, Person* data);
Person* last(Queue* queue);
Person* index_of_from(Queue* queue, int index);


#endif //PPII2_GRP_30_QUEUE_H
