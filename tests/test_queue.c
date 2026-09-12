#include "cds/queue.h"

#include <assert.h>
#include <stdio.h>

static void test_create_and_empty(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 4);
    assert(q != NULL);
    assert(cds_queue_len(q) == 0);
    assert(cds_queue_cap(q) == 4);
    assert(cds_queue_is_empty(q));
    assert(!cds_queue_is_full(q));

    int out;
    assert(!cds_queue_dequeue(q, &out));
    assert(!cds_queue_peek(q, &out));
 
    cds_queue_destroy(q);
}

static void test_create_rejects_zero_args(void) {
    assert(cds_queue_create(0, 4) == NULL);
    assert(cds_queue_create(sizeof(int), 0) == NULL);
}

static void test_fifo_order(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 4);
 
    for (int i = 1; i <= 3; i++) {
        assert(cds_queue_enqueue(q, &i));
    }
    assert(cds_queue_len(q) == 3);
 
    int out;
    assert(cds_queue_dequeue(q, &out) && out == 1);
    assert(cds_queue_dequeue(q, &out) && out == 2);
    assert(cds_queue_dequeue(q, &out) && out == 3);
    assert(cds_queue_is_empty(q));
 
    cds_queue_destroy(q);
}

static void test_full_rejects_enqueue(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 3);
 
    for (int i = 0; i < 3; i++) {
        assert(cds_queue_enqueue(q, &i));
    }
    assert(cds_queue_is_full(q));
    assert(cds_queue_len(q) == 3);
 
    int extra = 99;
    assert(!cds_queue_enqueue(q, &extra));
    assert(cds_queue_len(q) == 3);

    int out;
    assert(cds_queue_dequeue(q, &out) && out == 0);
    assert(cds_queue_dequeue(q, &out) && out == 1);
    assert(cds_queue_dequeue(q, &out) && out == 2);
    assert(cds_queue_is_empty(q));
 
    cds_queue_destroy(q);
}

static void test_peek_does_not_mutate(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 4);
 
    int a = 10, b = 20;
    assert(cds_queue_enqueue(q, &a));
    assert(cds_queue_enqueue(q, &b));
 
    int out;
    assert(cds_queue_peek(q, &out) && out == 10);
    assert(cds_queue_len(q) == 2);
    assert(cds_queue_peek(q, &out) && out == 10);
    assert(cds_queue_len(q) == 2);
 
    assert(cds_queue_dequeue(q, &out) && out == 10);
    assert(cds_queue_peek(q, &out) && out == 20);
 
    cds_queue_destroy(q);
}

static void test_wraparound_fifo(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 3);
 
    int v;
    v = 1; assert(cds_queue_enqueue(q, &v));
    v = 2; assert(cds_queue_enqueue(q, &v));
    v = 3; assert(cds_queue_enqueue(q, &v));
 
    int out;
    assert(cds_queue_dequeue(q, &out) && out == 1);
    assert(cds_queue_dequeue(q, &out) && out == 2);
 
    v = 4; assert(cds_queue_enqueue(q, &v));
    v = 5; assert(cds_queue_enqueue(q, &v));
    assert(cds_queue_is_full(q));
 
    assert(cds_queue_dequeue(q, &out) && out == 3);
    assert(cds_queue_dequeue(q, &out) && out == 4);
    assert(cds_queue_dequeue(q, &out) && out == 5);
    assert(cds_queue_is_empty(q));
 
    cds_queue_destroy(q);
}

static void test_wraparound_hammer(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 3);
 
    int next_in = 0; 
    int expect_out = 0; 
    int out;
 
    for (int round = 0; round < 1000; round++) {
        for (int k = 0; k < 2 && !cds_queue_is_full(q); k++) {
            assert(cds_queue_enqueue(q, &next_in));
            next_in++;
        }
        for (int k = 0; k < 2 && !cds_queue_is_empty(q); k++) {
            assert(cds_queue_dequeue(q, &out) && out == expect_out);
            expect_out++;
        }
    }
    while (!cds_queue_is_empty(q)) {
        assert(cds_queue_dequeue(q, &out) && out == expect_out);
        expect_out++;
    }
 
    assert(expect_out == next_in);
 
    cds_queue_destroy(q);
}

static void test_dequeue_with_null_out(void) {
    cds_queue *q = cds_queue_create(sizeof(int), 4);
 
    int x = 7;
    assert(cds_queue_enqueue(q, &x));
    assert(cds_queue_len(q) == 1);
 
    assert(cds_queue_dequeue(q, NULL));  /* discard */
    assert(cds_queue_is_empty(q));
 
    cds_queue_destroy(q);
}

static void test_struct_elements(void) {
    struct point {
        double x;
        double y;
    };
 
    cds_queue *q = cds_queue_create(sizeof(struct point), 2);
 
    struct point p1 = {1.5, 2.5};
    struct point p2 = {-3.0, 4.25};
    assert(cds_queue_enqueue(q, &p1));
    assert(cds_queue_enqueue(q, &p2));
    assert(cds_queue_is_full(q));
 
    struct point got;
    assert(cds_queue_peek(q, &got));
    assert(got.x == 1.5 && got.y == 2.5);
 
    assert(cds_queue_dequeue(q, &got) && got.x == 1.5 && got.y == 2.5);
    assert(cds_queue_dequeue(q, &got) && got.x == -3.0 && got.y == 4.25);
    assert(cds_queue_is_empty(q));
 
    cds_queue_destroy(q);
}

int main(void) {
    test_create_and_empty();
    test_create_rejects_zero_args();
    test_fifo_order();
    test_full_rejects_enqueue();
    test_peek_does_not_mutate();
    test_wraparound_fifo();
    test_wraparound_hammer();
    test_dequeue_with_null_out();
    test_struct_elements();
 
    printf("all queue tests passed\n");
    return 0;
}