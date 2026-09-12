#ifndef CDS_QUEUE_H
#define CDS_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cds_queue cds_queue;

cds_queue *cds_queue_create(size_t elem_size, size_t capacity);

void cds_queue_destroy(cds_queue *q);

size_t cds_queue_len(const cds_queue *q);

size_t cds_queue_cap(const cds_queue *q);

bool cds_queue_is_empty(const cds_queue *q);

bool cds_queue_is_full(const cds_queue *q);

bool cds_queue_enqueue(cds_queue *q, const void *elem);

bool cds_queue_dequeue(cds_queue *q, void *out);

bool cds_queue_peek(const cds_queue *q, void *out);

#ifdef __cplusplus
}
#endif

#endif