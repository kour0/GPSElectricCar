//
// Created by kour0 on 5/10/23.
//

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* create_queue(Person* data) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->data = data;
    queue->next = NULL;
    queue->prev = NULL;
    return queue;
}

void del_person(Queue* queue) {
    queue->prev->next = queue->next;
}

void push(Queue* queue, Person* data) {
    Queue* new_queue = create_queue(data);
    new_queue->next = NULL;
    new_queue->prev = queue;

    if (queue == NULL) {
        queue = new_queue;
        return;
    }

    while (queue->next != NULL) {
        queue = queue->next;
    }
    queue->next = new_queue;
}

Person* last(Queue* queue) {
    while (queue->next != NULL) {
        queue = queue->next;
    }
    return queue->data;
}
