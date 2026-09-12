#include "cds/list.h"

#include <assert.h>
#include <stdlib.h>

struct cds_list_node {
    void *payload;
    struct cds_list_node *next;
};

struct cds_list {
    struct cds_list_node *head;
    struct cds_list_node *tail;
    size_t len;
    cds_free_fn free_payload;
};

cds_list *cds_list_create(cds_free_fn free_payload) {
    cds_list *list = malloc(sizeof(*list));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->free_payload = free_payload;

    return list;
}

void cds_list_destroy(cds_list *list) {
    if (list == NULL) {
        return;
    }
    struct cds_list_node *itr = list->head;
    while (itr != NULL) {
        struct cds_list_node *next = itr->next;
        if (list ->free_payload != NULL) {
            list->free_payload(itr->payload);
        }
        free(itr);
        itr = next;
    }
    free(list);
}

size_t cds_list_len(const cds_list *list) {
    assert(list != NULL);
    return list->len;
}

bool cds_list_push_front(cds_list *list, void *payload) {
    assert(list != NULL);

    struct cds_list_node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return false;
    }

    new_node->payload = payload;
    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->len++;
    return true;
}

bool cds_list_push_back(cds_list *list, void *payload) {
    assert(list != NULL);

    struct cds_list_node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return false;
    }
    new_node->payload = payload;
    new_node->next = NULL;

    if (list->tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->len++;
    return true;
}

bool cds_list_pop_front(cds_list *list, void **out) {
    assert(list != NULL);

    if (list->head == NULL) {
        return false;
    }
    struct cds_list_node *popped_node = list->head;
    if (out != NULL) {
        *out = popped_node->payload; 
    }

    list->head = list->head->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }

    free(popped_node);
    list->len--;
    return true;
}

bool cds_list_pop_back(cds_list *list, void **out) {
    assert(list != NULL);

    if (list->tail == NULL) {
        return false;
    }
    struct cds_list_node *popped_node = list->tail;
    if (out != NULL) {
        *out = popped_node->payload;
    }
    
    if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        struct cds_list_node *prev = list->head;
        while(prev->next != list->tail) {
            prev = prev->next;
        }
        prev->next = NULL;
        list->tail = prev;
    }
    free(popped_node);
    list->len--;
    return true;
}

bool cds_list_remove(cds_list *list, void *payload) {
    assert(list != NULL);

    struct cds_list_node **ppNode = &list->head;
    struct cds_list_node *prev = NULL;

    while (*ppNode != NULL) {
        struct cds_list_node *node = *ppNode;
        if (node->payload == payload) {
            *ppNode = node->next;

            if (node == list->tail) {
                list->tail = prev;
            }

            if (list->free_payload != NULL) {
                list->free_payload(node->payload);
            }
            free(node);
            list->len--;
            return true;
        }
        prev = node;
        ppNode = &node->next;
    }
    return false;
}

void cds_list_foreach(const cds_list *list, cds_list_visit_fn visit, void *ctx) {
    assert(list != NULL);
    assert(visit != NULL);

    for (struct cds_list_node *node = list->head; node != NULL; node = node->next) {
        if (!visit(node->payload, ctx)) { break; }
    }
}


