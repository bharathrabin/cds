#include "cds/list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int *new_int(int val) {
    int *p = malloc(sizeof(int));
    assert(p != NULL);
    *p = val;
    return p;
}

static void test_create_and_destroy(void) {
    cds_list *list = cds_list_create(NULL);
    assert(list != NULL);
    assert(cds_list_len(list) == 0);

    cds_list_destroy(list);
}

static void test_push_front_order(void) {
    cds_list *list = cds_list_create(NULL);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
 
    assert(cds_list_push_front(list, a));
    assert(cds_list_push_front(list, b));
    assert(cds_list_push_front(list, c));
    assert(cds_list_len(list) == 3);
 
    void *out;
    assert(cds_list_pop_front(list, &out) && out == c);
    assert(cds_list_pop_front(list, &out) && out == b);
    assert(cds_list_pop_front(list, &out) && out == a);
    assert(cds_list_len(list) == 0);
 
    free(a);
    free(b);
    free(c);
    cds_list_destroy(list);
}

static void test_push_back_order(void) {
    cds_list *list = cds_list_create(NULL);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
 
    assert(cds_list_push_back(list, a));
    assert(cds_list_push_back(list, b));
    assert(cds_list_push_back(list, c));
    assert(cds_list_len(list) == 3);
 
    void *out;
    assert(cds_list_pop_front(list, &out) && out == a);
    assert(cds_list_pop_front(list, &out) && out == b);
    assert(cds_list_pop_front(list, &out) && out == c);
    assert(cds_list_len(list) == 0);
 
    free(a);
    free(b);
    free(c);
    cds_list_destroy(list);
}

static void test_pop_front_empty(void) {
    cds_list *list = cds_list_create(NULL);
 
    void *out;
    assert(!cds_list_pop_front(list, &out));
    assert(cds_list_len(list) == 0);
 
    cds_list_destroy(list);
}

static void test_pop_front_to_empty_and_reuse(void) {
    cds_list *list = cds_list_create(NULL);
 
    int *a = new_int(1);
    assert(cds_list_push_back(list, a));
 
    void *out;
    assert(cds_list_pop_front(list, &out) && out == a);
    assert(cds_list_len(list) == 0);
    free(a);
 
    int *b = new_int(2);
    assert(cds_list_push_back(list, b));
    assert(cds_list_len(list) == 1);
    assert(cds_list_pop_front(list, &out) && out == b);
    free(b);
 
    cds_list_destroy(list);
}

static void test_pop_back(void) {
    cds_list *list = cds_list_create(NULL);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
    assert(cds_list_push_back(list, a));
    assert(cds_list_push_back(list, b));
    assert(cds_list_push_back(list, c));
 
    void *out;
    assert(cds_list_pop_back(list, &out) && out == c);
    assert(cds_list_len(list) == 2);
 
    int *d = new_int(4);
    assert(cds_list_push_back(list, d)); /* now a,b,d */
    assert(cds_list_pop_back(list, &out) && out == d);
    assert(cds_list_pop_back(list, &out) && out == b);
    assert(cds_list_pop_back(list, &out) && out == a);
    assert(cds_list_len(list) == 0);
 
    assert(!cds_list_pop_back(list, &out));
 
    free(a);
    free(b);
    free(c);
    free(d);
    cds_list_destroy(list);
}

static void test_remove(void) {
    /* This test uses a real destructor: cds_list_remove DESTROYS the
     * removed payload (unlike pop, which hands it back). */
    cds_list *list = cds_list_create(free);
 
    int *a = new_int(1);
    int *b = new_int(2);
    int *c = new_int(3);
    assert(cds_list_push_back(list, a)); /* a,b,c */
    assert(cds_list_push_back(list, b));
    assert(cds_list_push_back(list, c));
 
    assert(cds_list_remove(list, a)); /* -> b,c */
    assert(cds_list_len(list) == 2);
 
    assert(cds_list_remove(list, c)); /* -> b */
    assert(cds_list_len(list) == 1);
 
    int *d = new_int(4);
    assert(cds_list_push_back(list, d)); /* -> b,d, proving tail was b */
    assert(cds_list_len(list) == 2);
 
    assert(cds_list_remove(list, b)); /* -> d */
    assert(cds_list_len(list) == 1);
 
    int stray = 99;
    assert(!cds_list_remove(list, &stray));
    assert(cds_list_len(list) == 1);
 
    assert(cds_list_remove(list, d)); /* -> empty */
    assert(cds_list_len(list) == 0);
 
    cds_list_destroy(list);
}

static bool sum_visitor(void *payload, void *ctx) {
    int *total = ctx;
    *total += *(int *)payload;
    return true;
}

struct find_ctx {
    int target;
    int visited;
    bool found;
};

static bool find_visitor(void *payload, void *ctx) {
    struct find_ctx *fc = ctx;
    fc->visited++;
    if (*(int *)payload == fc->target) {
        fc->found = true;
        return false; /* stop the traversal early */
    }
    return true;
}

static void test_foreach(void) {
    cds_list *list = cds_list_create(free);
 
    assert(cds_list_push_back(list, new_int(10)));
    assert(cds_list_push_back(list, new_int(20)));
    assert(cds_list_push_back(list, new_int(30)));
 
    int total = 0;
    cds_list_foreach(list, sum_visitor, &total);
    assert(total == 60);
 
    struct find_ctx fc = {.target = 20, .visited = 0, .found = false};
    cds_list_foreach(list, find_visitor, &fc);
    assert(fc.found);
    assert(fc.visited == 2);
 
    cds_list_destroy(list);
}

static void test_destructor_frees_payloads(void) {
    cds_list *list = cds_list_create(free);
 
    for (int i = 0; i < 5; i++) {
        assert(cds_list_push_back(list, new_int(i)));
    }
    assert(cds_list_len(list) == 5);
 
    cds_list_destroy(list); /* frees all 5 payloads + all nodes */
}

static void test_pop_does_not_free_payload(void) {
    cds_list *list = cds_list_create(free);
 
    int *a = new_int(42);
    assert(cds_list_push_back(list, a));
 
    void *out;
    assert(cds_list_pop_front(list, &out));
    assert(out == a);
    assert(*(int *)out == 42); /* still valid memory: pop didn't free it */
 
    /* Since pop transferred ownership to us, WE free it now. */
    free(out);
 
    cds_list_destroy(list); /* list is empty; nothing to free */
}

int main(void) {
    test_create_and_destroy();
    test_push_front_order();
    test_push_back_order();
    test_pop_front_empty();
    test_pop_front_to_empty_and_reuse();
    test_pop_back();
    test_remove();
    test_foreach();
    test_destructor_frees_payloads();
    test_pop_does_not_free_payload();
 
    printf("all list tests passed\n");
    return 0;
}
 