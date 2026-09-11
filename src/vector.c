#include <cds/vector.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct cds_vector {
    void *data; /* byte buffer of size cap * elem_size bytes*/
    size_t elem_size; /* bytes per element, fixed at construction time */
    size_t len; /* elements currently stored */
    size_t cap; /* elements the buffer can hold before growing */
};

#define CDS_VECTOR_INITIAL_CAP 4
#define CDS_VECTOR_GROWTH_FACTOR 2

cds_vector *cds_vector_create(size_t elem_size) {
    if (elem_size == 0) { 
        return NULL; 
    }
    cds_vector *v = malloc(sizeof(*v));
    if (v == NULL) { 
        return NULL; 
    }

    /* Lazy allocation: no buffer until the first push */
    v->data = NULL;
    v->elem_size = elem_size;
    v->len = 0;
    v->cap = 0;

    return v;
}

void cds_vector_destroy(cds_vector *v) {
    if (v == NULL) {
        return;
    }
    free(v->data);
    free(v);
}

size_t cds_vector_len(const cds_vector *v) {
    assert(v != NULL);
    return v->len;
}

size_t cds_vector_cap(const cds_vector *v) {
    assert(v != NULL);
    return v->cap;
}

bool cds_vector_reserve(cds_vector *v, size_t min_cap) {
    assert(v != NULL);

    if (min_cap <= v->cap) {
        return true;
    }

    /* Guard against abnormally large capacities that can cause wrap around */
    if (min_cap > SIZE_MAX / v->elem_size) {
        return false;
    }

    void *new_data = realloc(v->data, min_cap * v->elem_size);
    if (new_data == NULL) {
        return false;
    }

    v->data = new_data;
    v->cap = min_cap;
    return true;
}

static bool cds_vector_grow_if_full(cds_vector *v) {
    if (v->len < v->cap) {
        return true;
    }
    size_t new_cap = (v->cap == 0) ? CDS_VECTOR_INITIAL_CAP : v->cap * CDS_VECTOR_GROWTH_FACTOR;
    return cds_vector_reserve(v, new_cap);
}

bool cds_vector_push(cds_vector *v, const void *elem) {
    assert(v != NULL);
    assert(elem != NULL);

    if (!cds_vector_grow_if_full(v)) {
        return false;
    }

    void *dest = (char *)v->data + (v->len * v->elem_size);
    memcpy(dest, elem, v->elem_size);
    v->len++;

    return true;
}

bool cds_vector_pop(cds_vector *v, void *out) {
    assert(v != NULL);

    if (v->len == 0) {
        return false;
    }

    v->len--;

    if (out != NULL) {
        void *src = (char*)v->data + (v->len * v->elem_size);
        memcpy(out, src, v->elem_size);
    }
    return true;
}

void *cds_vector_get(const cds_vector *v, size_t index) {
    assert(v != NULL);

    if (index >= v->len) {
        return NULL;
    }
    return (char*)v->data + (index * v->elem_size);
}

bool cds_vector_set(cds_vector *v, size_t index, const void *elem) {
    assert(v != NULL);
    assert(elem != NULL);

    if (index >= v->len) {
        return false;
    }

    void *dest = (char *)v->data + (index * v->elem_size);
    memcpy(dest, elem, v->elem_size);

    return true;
}

