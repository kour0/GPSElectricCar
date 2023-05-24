#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* createQueue(void) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->data = NULL;
    queue->next = NULL;
    queue->prev = NULL;
    return queue;
}

void del_person(Queue* queue) {
    queue->prev->next = queue->next;
}

/*void push(Queue* queue, Person* data) {

    float time = data->remainingTime;

    if (queue->data == NULL) {
        queue->data = data;
        return;
    }

    Queue* new_queue = create_queue();
    new_queue->data = data;

    while (queue->next != NULL) {
        if (queue->data->remainingTime < time) {
            queue->prev->next = new_queue;
            new_queue->prev = queue->prev;
            new_queue->next = queue;
            queue->prev = new_queue;
            return;
        }
        queue = queue->next;
    }
    queue->next = new_queue;
    new_queue->prev = queue;
}*/

void push(Queue* queue, Person* person, int timeOffset) {
    if (queue->data == NULL) {
        queue->data = person;
        return;
    }

    Queue* new_queue = createQueue();
    new_queue->data = person;

    while (queue->next != NULL) {
        queue = queue->next;
    }
    queue->next = new_queue;
    new_queue->prev = queue;

    person->waitingTime = queue->data->waitingTime + queue->data->chargingTime - timeOffset;

}

Person* last(Queue* queue) {
    while (queue->next != NULL) {
        queue = queue->next;
    }
    return queue->data;
}

Person* index_of_from(Queue* queue, int index) {
    for (int i = 0; i < index; i++) {
        queue = queue->prev;
    }
    return queue->data;
}

int timeToWait(Queue* queue) {
    while (queue->next != NULL) {
        queue = queue->next;
    }
    if (queue->data == NULL) {
        return 0;
    }
    return queue->data->waitingTime + queue->data->chargingTime;
}

Person* first(Queue* queue) {
    return queue->data;
}

void pop(Queue* queue) {
    if (queue->next == NULL) {
        queue->data = NULL;
        return;
    }

    Queue next = *queue->next;
    free(queue->next);
    queue->data = next.data;
    queue->next = next.next;
    next.next->prev = queue;
}
