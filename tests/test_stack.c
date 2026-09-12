#include "cds/stack.h"

#include <assert.h>
#include <stdio.h>

static void test_create_and_empty(void) {
    cds_stack *s = cds_stack_create(sizeof(int));
    assert(s != NULL);
    assert(cds_stack_len(s) == 0);
    assert(cds_stack_is_empty(s));

    int out;
    assert(!cds_stack_peek(s, &out));
    assert(!cds_stack_pop(s, &out));
 
    cds_stack_destroy(s);
}

static void test_lifo_order(void) {
    cds_stack *s = cds_stack_create(sizeof(int));
 
    for (int i = 1; i <= 3; i++) {
        assert(cds_stack_push(s, &i));
    }
    assert(cds_stack_len(s) == 3);
    assert(!cds_stack_is_empty(s));
 
    int out;
    assert(cds_stack_pop(s, &out) && out == 3); 
    assert(cds_stack_pop(s, &out) && out == 2);
    assert(cds_stack_pop(s, &out) && out == 1);
    assert(cds_stack_is_empty(s));
 
    cds_stack_destroy(s);
}

static void test_peek_does_not_mutate(void) {
    cds_stack *s = cds_stack_create(sizeof(int));
 
    int a = 10, b = 20;
    assert(cds_stack_push(s, &a));
    assert(cds_stack_push(s, &b));
    int out;
    assert(cds_stack_peek(s, &out) && out == 20);
    assert(cds_stack_len(s) == 2);        
    assert(cds_stack_peek(s, &out) && out == 20);
    assert(cds_stack_len(s) == 2);
 
    assert(cds_stack_pop(s, &out) && out == 20);
    assert(cds_stack_peek(s, &out) && out == 10);
 
    cds_stack_destroy(s);
}

static void test_peek_copies_not_aliases(void) {
    cds_stack *s = cds_stack_create(sizeof(int));
 
    int first = 7;
    assert(cds_stack_push(s, &first));
 
    int saved;
    assert(cds_stack_peek(s, &saved) && saved == 7);
 
    for (int i = 0; i < 100; i++) {
        assert(cds_stack_push(s, &i));
    }
 
    assert(saved == 7);
 
    cds_stack_destroy(s);
}

static void test_pop_with_null_out(void) {
    cds_stack *s = cds_stack_create(sizeof(int));
 
    int x = 99;
    assert(cds_stack_push(s, &x));
    assert(cds_stack_len(s) == 1);
 
    assert(cds_stack_pop(s, NULL));
    assert(cds_stack_len(s) == 0);
    assert(cds_stack_is_empty(s));
 
    cds_stack_destroy(s);
}

static void test_struct_elements(void) {
    struct point {
        double x;
        double y;
    };
 
    cds_stack *s = cds_stack_create(sizeof(struct point));
 
    struct point p1 = {1.5, 2.5};
    struct point p2 = {-3.0, 4.25};
    assert(cds_stack_push(s, &p1));
    assert(cds_stack_push(s, &p2));
 
    struct point got;
    assert(cds_stack_peek(s, &got));
    assert(got.x == -3.0 && got.y == 4.25);
 
    assert(cds_stack_pop(s, &got));
    assert(got.x == -3.0 && got.y == 4.25);
    assert(cds_stack_pop(s, &got));
    assert(got.x == 1.5 && got.y == 2.5);
    assert(cds_stack_is_empty(s));
 
    cds_stack_destroy(s);
}

int main(void) {
    test_create_and_empty();
    test_lifo_order();
    test_peek_does_not_mutate();
    test_peek_copies_not_aliases();
    test_pop_with_null_out();
    test_struct_elements();
 
    printf("all stack tests passed\n");
    return 0;
}