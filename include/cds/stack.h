#ifndef CDS_STACK_H
#define CDS_STACK_H


#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* A stack of fixed-sized elements */
typedef struct cds_stack cds_stack;

cds_stack *cds_stack_create(size_t elem_size);

void cds_stack_destroy(cds_stack *s);

size_t cds_stack_len(const cds_stack *s);

bool cds_stack_is_empty(const cds_stack *s);

bool cds_stack_push(cds_stack *s, const void *elem);

bool cds_stack_pop(cds_stack *s, void *out);

bool cds_stack_peek(const cds_stack *s, void *out);

#ifdef __cplusplus
}
#endif

#endif