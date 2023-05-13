#include "queue.h"
#include <stdlib.h>

/* Trivial implementation with a single-linked list. */

typedef struct elem {
    struct elem *next;
    void *val;
} *queue_t;

void enqueue(queue_t *queue, void *val) {
    queue_t e = malloc(sizeof(struct elem));
    e->val = val;
    e->next = NULL;
    if (*queue == NULL) {
        *queue = e;
    } else {
        queue_t p = *queue;
        while (p->next != NULL) p = p->next;
        p->next = e;
    }
}

void *dequeue(queue_t *queue) {
    queue_t head = *queue;
    void *val = head->val;
    *queue = (*queue)->next;
    free(head);
    return val;
}

int isempty(queue_t *queue) {
    return (*queue == NULL);
}

int length(queue_t *queue) {
    int l = 0;
    queue_t p = *queue;
    while (p != NULL) { p = p-> next; l++; }
    return l;
}

