#include "cds/dlist.h"
 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int *new_int(int value) {
    int *p = malloc(sizeof(int));
    assert(p != NULL);
    *p = value;
    return p;
}

static void test_create_and_destroy(void) {
    cds_dlist *list = cds_dlist_create(NULL);
    assert(list != NULL);
    assert(cds_dlist_len(list) == 0);
 
    cds_dlist_destroy(list);
    cds_dlist_destroy(NULL); /* safe no-op */
}

static void test_empty_list_fails_cleanly(void) {
    cds_dlist *list = cds_dlist_create(NULL);
 
    void *out;
    assert(!cds_dlist_pop_front(list, &out));
    assert(!cds_dlist_pop_back(list, &out));
    assert(cds_dlist_len(list) == 0);
 
    cds_dlist_destroy(list);
}

static void test_push_both_ends_order(void) {
    cds_dlist *list = cds_dlist_create(NULL);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
 
    assert(cds_dlist_push_back(list, a));  
    assert(cds_dlist_push_back(list, b)); 
    assert(cds_dlist_push_front(list, c));
    assert(cds_dlist_len(list) == 3);
 
    void *out;
    assert(cds_dlist_pop_front(list, &out) && out == c);
    assert(cds_dlist_pop_back(list, &out) && out == b);
    assert(cds_dlist_len(list) == 1);
    assert(cds_dlist_pop_front(list, &out) && out == a);
    assert(cds_dlist_len(list) == 0);
 
    free(a);
    free(b);
    free(c);
    cds_dlist_destroy(list);
}

static void test_pop_to_empty_and_reuse(void) {
    cds_dlist *list = cds_dlist_create(NULL);
 
    int *a = new_int(1);
    assert(cds_dlist_push_back(list, a));
    void *out;
    assert(cds_dlist_pop_front(list, &out) && out == a);
    assert(cds_dlist_len(list) == 0);
    free(a);
 
    int *b = new_int(2);
    int *c = new_int(3);
    assert(cds_dlist_push_back(list, b));
    assert(cds_dlist_push_back(list, c));
    assert(cds_dlist_len(list) == 2);
    assert(cds_dlist_pop_back(list, &out) && out == c);
    assert(cds_dlist_pop_back(list, &out) && out == b);
    assert(cds_dlist_len(list) == 0);
    free(b);
    free(c);
 
    cds_dlist_destroy(list);
}

static void test_remove(void) {
    cds_dlist *list = cds_dlist_create(free);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
    assert(cds_dlist_push_back(list, a));
    assert(cds_dlist_push_back(list, b));
    assert(cds_dlist_push_back(list, c));
 
    /* Remove the HEAD. */
    assert(cds_dlist_remove(list, a));
    assert(cds_dlist_len(list) == 2);

    assert(cds_dlist_remove(list, c));
    assert(cds_dlist_len(list) == 1);
 
    int *d = new_int(4);
    assert(cds_dlist_push_back(list, d));
    assert(cds_dlist_len(list) == 2);
 
    int stray = 99;
    assert(!cds_dlist_remove(list, &stray));
    assert(cds_dlist_len(list) == 2);
 
    assert(cds_dlist_remove(list, b));
    assert(cds_dlist_remove(list, d));
    assert(cds_dlist_len(list) == 0);
 
    cds_dlist_destroy(list);
}

struct collect_ctx {
    int values[16];
    int count;
};

static bool collect_visitor(void *payload, void *ctx) {
    struct collect_ctx *c = ctx;
    c->values[c->count++] = *(int *)payload;
    return true;
}

static void test_foreach_both_directions(void) {
    cds_dlist *list = cds_dlist_create(free);
 
    assert(cds_dlist_push_back(list, new_int(10)));
    assert(cds_dlist_push_back(list, new_int(20)));
    assert(cds_dlist_push_back(list, new_int(30)));
 
    struct collect_ctx forward = {.count = 0};
    cds_dlist_foreach(list, collect_visitor, &forward);
    assert(forward.count == 3);
    assert(forward.values[0] == 10 && forward.values[1] == 20 &&
           forward.values[2] == 30);
 
    struct collect_ctx backward = {.count = 0};
    cds_dlist_foreach_reverse(list, collect_visitor, &backward);
    assert(backward.count == 3);
    assert(backward.values[0] == 30 && backward.values[1] == 20 &&
           backward.values[2] == 10);
 
    cds_dlist_destroy(list);
}

static void test_destructor_frees_payloads(void) {
    cds_dlist *list = cds_dlist_create(free);
 
    for (int i = 0; i < 5; i++) {
        assert(cds_dlist_push_back(list, new_int(i)));
    }
    assert(cds_dlist_len(list) == 5);
 
    cds_dlist_destroy(list);
}

static void test_pop_does_not_free_payload(void) {
    cds_dlist *list = cds_dlist_create(free);
 
    int *a = new_int(42);
    assert(cds_dlist_push_back(list, a));
 
    void *out;
    assert(cds_dlist_pop_front(list, &out));
    assert(out == a);
    assert(*(int *)out == 42);
 
    free(out);
 
    cds_dlist_destroy(list);
}

static void test_pop_back_is_correct_at_scale(void) {
    cds_dlist *list = cds_dlist_create(free);
 
    for (int i = 0; i < 500; i++) {
        assert(cds_dlist_push_back(list, new_int(i)));
    }
    assert(cds_dlist_len(list) == 500);
 
    void *out;
    for (int i = 499; i >= 0; i--) {
        assert(cds_dlist_pop_back(list, &out));
        assert(*(int *)out == i);
        free(out);
    }
    assert(cds_dlist_len(list) == 0);
 
    cds_dlist_destroy(list);
}

int main(void) {
    test_create_and_destroy();
    test_empty_list_fails_cleanly();
    test_push_both_ends_order();
    test_pop_to_empty_and_reuse();
    test_remove();
    test_foreach_both_directions();
    test_destructor_frees_payloads();
    test_pop_does_not_free_payload();
    test_pop_back_is_correct_at_scale();
 
    printf("all dlist tests passed\n");
    return 0;
}


