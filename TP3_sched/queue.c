#include "queue.h"
#include <stdlib.h>

/* Trivial (and inefficient) implementation with a single-linked list. */

typedef struct elem {
    struct elem *next;
    void *val;
} *queue_t;

void queue_add(queue_t *queue, void *val) {
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

void *queue_take(queue_t *queue) {
    queue_t head = *queue;
    void *val = head->val;
    *queue = (*queue)->next;
    free(head);
    return val;
}

int queue_isempty(queue_t *queue) {
    return (*queue == NULL);
}

int queue_length(queue_t *queue) {
    int l = 0;
    queue_t p = *queue;
    while (p != NULL) { p = p-> next; l++; }
    return l;
}

void *queue_get(queue_t *queue, int position)
{
    queue_t p = *queue;
    while ((p != NULL) && (position > 0)) { p = p->next; position--; }
    if (p != NULL)
      return p->val;
    else
      return NULL;
}

void *queue_remove(queue_t *queue, int position) {
    queue_t p = *queue;
    queue_t prec = NULL;
    while ((p != NULL)  && (position > 0)) { prec = p; p = p->next; position--; }
    if (p != NULL) {
        void *val = p->val;
        if (prec == NULL)
          *queue = p->next;
        else
          prec->next = p->next;
        free(p);
        return val;
    } else {
        return NULL;
    }
} 

