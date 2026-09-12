#ifndef CDS_LIST_H
#define CDS_LIST_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cds_list cds_list;

typedef void (*cds_free_fn)(void *payload);

cds_list *cds_list_create(cds_free_fn free_payload);

void cds_list_destroy(cds_list *list);

size_t cds_list_len(const cds_list *list);

bool cds_list_push_front(cds_list *list, void *payload);

bool cds_list_push_back(cds_list *list, void *payload);

bool cds_list_pop_front(cds_list *list, void **out);

bool cds_list_pop_back(cds_list *list, void **out);

bool cds_list_remove(cds_list *list, void *payload);

typedef bool (*cds_list_visit_fn)(void *payload, void *ctx);

void cds_list_foreach(const cds_list *list, cds_list_visit_fn visit, void *ctx);

#ifdef __cplusplus
}
#endif

#endif