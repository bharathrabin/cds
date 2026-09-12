#include "cds/queue.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct cds_queue {
    void *data;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
};

cds_queue *cds_queue_create(size_t elem_size, size_t capacity) {
    if (elem_size == 0 || capacity == 0) {
        return NULL;
    }

    cds_queue *q = malloc(sizeof(*q));
    if (q == NULL) { 
        return NULL; 
    }
    if (capacity > SIZE_MAX / elem_size) {
        free(q);
        return NULL;
    }
    q->data = malloc(capacity * elem_size);
    if (q->data == NULL) {
        free(q);
        return NULL;
    }
    q->elem_size = elem_size;
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    return q;
}

void cds_queue_destroy(cds_queue *q) {
    if (q == NULL) { return; }
    free(q->data);
    free(q);
}

size_t cds_queue_len(const cds_queue *q) {
    assert(q != NULL);
    return q->count;
}

size_t cds_queue_cap(const cds_queue *q) {
    assert(q != NULL);
    return q->capacity;
}

bool cds_queue_is_empty(const cds_queue *q) {
    assert(q != NULL);
    return q->count == 0;
}

bool cds_queue_is_full(const cds_queue *q) {
    assert(q != NULL);
    return q->count == q->capacity;
}

static void *slot_at(const cds_queue *q, size_t index) {
    return (char *)q->data + (index * q->elem_size);
}

bool cds_queue_enqueue(cds_queue *q, const void *elem) {
    assert(q != NULL);
    assert(elem != NULL);

    if (q->count == q->capacity) {
        return false;
    }

    memcpy(slot_at(q, q->tail), elem, q->elem_size);
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
    
    return true;
}

bool cds_queue_dequeue(cds_queue *q, void *out) {
    assert(q != NULL);

    if (q->count == 0) {
        return false;
    }

    if (out != NULL) {
        memcpy(out, slot_at(q, q->head), q->elem_size);
    }
    q->head = (q->head + 1) % q->capacity;
    q->count--;

    return true;
}

bool cds_queue_peek(const cds_queue *q, void *out) {
    assert(q != NULL);

    if (q->count == 0) {
        return false;
    }

    if (out != NULL) {
        memcpy(out, slot_at(q, q->head), q->elem_size);
    }
    return true;
}