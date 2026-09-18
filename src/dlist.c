#include "cds/dlist.h"


#include <assert.h>
#include <stdlib.h>

struct cds_dlist_node {
    void *payload;
    struct cds_dlist_node *next;
    struct cds_dlist_node *prev;
};

struct cds_dlist {
    struct cds_dlist_node sentinel;
    cds_free_fn free_payload;
    size_t len;
};

static void insert_between(struct cds_dlist_node *before, struct cds_dlist_node *node, struct cds_dlist_node *after) {
    node->prev = before;
    node->next = after;
    before->next = node;
    after->prev = node;
}

static void unlink_node(struct cds_dlist_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

cds_dlist *cds_dlist_create(cds_free_fn free_payload) {
    cds_dlist *list = malloc(sizeof(*list));
    if (list == NULL) { return NULL; }
    list->sentinel.payload = NULL;
    list->sentinel.next = &list->sentinel;
    list->sentinel.prev = &list->sentinel;
    list->free_payload = free_payload;
    list->len = 0;

    return list;
}

void cds_dlist_destroy(cds_dlist *list) {
    if (list == NULL) { return; }
    struct cds_dlist_node *node = list->sentinel.next;
    while(node != &list->sentinel) {
        struct cds_dlist_node *next = node->next;
        if (list->free_payload != NULL) {
            list->free_payload(node->payload);
        }
        free(node);
        node = next;
    }
    free(list);
}

size_t cds_dlist_len(const cds_dlist *list) {
    assert(list != NULL);
    return list->len;
}

bool cds_dlist_push_front(cds_dlist *list, void *payload) {
    assert(list != NULL);

    struct cds_dlist_node *node = malloc(sizeof(*node));
    if (node == NULL) { return false; }
    node->payload = payload;

    insert_between(&list->sentinel, node, list->sentinel.next);
    list->len++;
    return true;
}

bool cds_dlist_push_back(cds_dlist *list, void *payload) {
    assert(list != NULL);

    struct cds_dlist_node *node = malloc(sizeof(*node));
    if (node == NULL) { return false; }
    node->payload = payload;
    
    insert_between(list->sentinel.prev, node, &list->sentinel);
    list->len++;
    return true;
}

bool cds_dlist_pop_front(cds_dlist *list, void **out) {
    assert(list != NULL);

    struct cds_dlist_node *node = list->sentinel.next;
    if (node == &list->sentinel) { return false; }
    if (out != NULL) {
        *out = node->payload;
    }
    unlink_node(node);
    free(node);
    list->len--;
    return true;
}

bool cds_dlist_pop_back(cds_dlist *list, void **out) {
    assert(list != NULL);

    struct cds_dlist_node *node = list->sentinel.prev;
    if (node == &list->sentinel) { return false; }
    if (out != NULL) {
        *out = node->payload;
    }
    unlink_node(node);
    free(node);
    list->len--;
    return true;
}

bool cds_dlist_remove(cds_dlist *list, void *payload) {
    assert(list != NULL);

    struct cds_dlist_node *node = list->sentinel.next;
    while(node != &list->sentinel) {
        if(node->payload == payload) { 
            unlink_node(node); 
            if(list->free_payload != NULL) { 
                list->free_payload(node->payload); 
            }
            free(node);
            list->len--;
            return true;
        }
        node = node->next;
    }
    return false;
}

void cds_dlist_foreach(const cds_dlist *list, cds_dlist_visit_fn visit, void *ctx) {
    assert(list != NULL);
    assert(visit != NULL);

    for(struct cds_dlist_node *node = list->sentinel.next; node != &list->sentinel; node= node->next){
        if(!visit(node->payload, ctx)) { break; }
    }
}

void cds_dlist_foreach_reverse(const cds_dlist *list, cds_dlist_visit_fn visit, void *ctx) {
    assert(list != NULL);
    assert(visit != NULL);

    for(struct cds_dlist_node *node = list->sentinel.prev; node != &list->sentinel; node= node->prev){
        if(!visit(node->payload, ctx)) { break; }
    }
}




