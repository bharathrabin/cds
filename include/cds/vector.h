#ifndef CDS_VECTOR_H
#define CDS_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cds_vector cds_vector;

cds_vector *cds_vector_create(size_t elem_size);

void cds_vector_destroy(cds_vector *v);

size_t cds_vector_len(const cds_vector *v);

size_t cds_vector_cap(const cds_vector *v);

bool cds_vector_reserve(cds_vector *v, size_t min_cap);

bool cds_vector_push(cds_vector *v, const void *elem);

bool cds_vector_pop(cds_vector *v, void *out);

void *cds_vector_get(const cds_vector *v, size_t index);

bool cds_vector_set(cds_vector *v, size_t index, const void *elem);

#ifdef __cplusplus
}
#endif

#endif