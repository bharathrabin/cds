#include "cds/vector.h"

#include <assert.h>
#include <stdio.h>

static void test_create_and_destroy(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);
    assert(cds_vector_len(v) == 0);
    assert(cds_vector_cap(v) == 0); /* lazy allocation: nothing yet */

    cds_vector_destroy(v);
}

static void test_push_and_get(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 3; i++) {
        assert(cds_vector_push(v, &i));
    }
    assert(cds_vector_len(v) == 3);

    for (int i = 0; i < 3; i++) {
        int *elem = cds_vector_get(v, (size_t)i);
        assert(elem != NULL);
        assert(*elem == i);
    }

    assert(cds_vector_get(v, 3) == NULL); /* one past the end: invalid */

    cds_vector_destroy(v);
}

static void test_growth(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);
    assert(cds_vector_cap(v) == 0);

    /* Push past the initial capacity (4) to force at least one
     * realloc. If the growth logic or the memcpy offsets were wrong,
     * this is where it would show up — either as a wrong value here,
     * or as an AddressSanitizer crash from stepping on freed memory. */
    for (int i = 0; i < 10; i++) {
        assert(cds_vector_push(v, &i));
    }

    assert(cds_vector_len(v) == 10);
    assert(cds_vector_cap(v) >= 10);

    for (int i = 0; i < 10; i++) {
        int *elem = cds_vector_get(v, (size_t)i);
        assert(*elem == i);
    }

    cds_vector_destroy(v);
}

static void test_pop(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        assert(cds_vector_push(v, &values[i]));
    }

    int out = 0;
    assert(cds_vector_pop(v, &out));
    assert(out == 30);
    assert(cds_vector_len(v) == 2);

    assert(cds_vector_pop(v, NULL)); /* caller allowed to discard the value */
    assert(cds_vector_len(v) == 1);

    assert(cds_vector_pop(v, &out));
    assert(out == 10);
    assert(cds_vector_len(v) == 0);

    assert(!cds_vector_pop(v, &out)); /* empty: must fail, not crash */

    cds_vector_destroy(v);
}

static void test_set(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);

    int a = 1, b = 2;
    assert(cds_vector_push(v, &a));
    assert(cds_vector_push(v, &b));

    int replacement = 99;
    assert(cds_vector_set(v, 0, &replacement));
    assert(*(int *)cds_vector_get(v, 0) == 99);

    assert(!cds_vector_set(v, 2, &replacement)); /* index == len: rejected */

    cds_vector_destroy(v);
}

static void test_reserve(void) {
    cds_vector *v = cds_vector_create(sizeof(int));
    assert(v != NULL);

    assert(cds_vector_reserve(v, 100));
    assert(cds_vector_cap(v) == 100);
    assert(cds_vector_len(v) == 0); /* reserve grows capacity, not length */

    assert(cds_vector_reserve(v, 10)); /* smaller than current cap: no-op */
    assert(cds_vector_cap(v) == 100);

    cds_vector_destroy(v);
}

int main(void) {
    test_create_and_destroy();
    test_push_and_get();
    test_growth();
    test_pop();
    test_set();
    test_reserve();

    printf("all vector tests passed\n");
    return 0;
}