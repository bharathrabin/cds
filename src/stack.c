#include "cds/stack.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cds/vector.h"

struct cds_stack {
    cds_vector *vec;
};

cds_stack *cds_stack_create(size_t elem_size) {
    cds_stack *s = malloc(sizeof(*s));
    if (s == NULL) {
        return NULL;
    }
    s->vec = cds_vector_create(elem_size);
    if (s->vec == NULL) {
        free(s);
        return NULL;
    }

    return s;
}

void cds_stack_destroy(cds_stack *s) {
    if (s == NULL) {
        return;
    }
    cds_vector_destroy(s->vec);
    free(s);
}

size_t cds_stack_len(const cds_stack *s) {
    assert(s != NULL);
    return cds_vector_len(s->vec);
}

bool cds_stack_is_empty(const cds_stack *s) {
    assert(s != NULL);
    return cds_vector_len(s->vec) == 0;
}

bool cds_stack_push(cds_stack *s, const void *elem) {
    assert(s != NULL);
    return cds_vector_push(s->vec, elem);
}

bool cds_stack_pop(cds_stack *s, void *out) {
    assert(s != NULL);
    return cds_vector_pop(s->vec, out);
}

bool cds_stack_peek(const cds_stack *s, void *out) {
    assert(s != NULL);

    size_t len = cds_vector_len(s->vec);
    if (len == 0) { return false; }
    const void *top = cds_vector_get(s->vec, len - 1);
    assert(top != NULL);

    if (out != NULL) {
        memcpy(out, top, cds_vector_elem_size(s->vec));
    }
    return true;
}

