#ifndef CDS_DLIST_H
#define CDS_DLIST_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cds_dlist cds_dlist;

typedef void (*cds_free_fn)(void *payload);

cds_dlist *cds_dlist_create(cds_free_fn payload);

void cds_dlist_destroy(cds_dlist *list);

size_t cds_dlist_len(const cds_dlist *list);

bool cds_dlist_push_front(cds_dlist *list, void *payload);

bool cds_dlist_push_back(cds_dlist *list, void *payload);

bool cds_dlist_pop_front(cds_dlist *list, void **out);

bool cds_dlist_pop_back(cds_dlist *list, void **out);

bool cds_dlist_remove(cds_dlist *list, void *payload);

typedef bool (*cds_dlist_visit_fn) (void *payload, void *ctx);

void cds_dlist_foreach(const cds_dlist *list, cds_dlist_visit_fn visit, void *ctx);

void cds_dlist_foreach_reverse(const cds_dlist *list, cds_dlist_visit_fn visit, void *ctx);

#ifdef __cplusplus
}
#endif

#endif