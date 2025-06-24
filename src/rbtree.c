#pragma once

// Red-Black Tree implementation based on Robert Sedgewick paper:
// Left-Leaning Red-Black Trees. Posted here, 2008.

// TODO: Add deletion.
// TODO: Add search.

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef int (*compare_f)(const void *p1, const void *p2);

typedef struct rbnode {
    struct rbnode *kids[2];
    struct rbnode *parent;
    bool is_black;
    char data[];
} rbnode_s;

typedef struct rbtree {
    rbnode_s *head;
    size_t size;
    compare_f cmp;
} rbtree_s;

static void rbnode_color_flip(rbnode_s *self) {
    self->is_black = !self->is_black;
    self->kids[0]->is_black = !self->kids[0]->is_black;
    self->kids[1]->is_black = !self->kids[1]->is_black;
}

static rbnode_s *rbnode_rotate(rbnode_s *self, bool to_right) {
    rbnode_s *up = self->kids[!to_right];
    self->kids[!to_right] = up->kids[to_right];
    up->kids[to_right] = self;
    up->is_black = self->is_black;
    self->is_black = false;
    return up;
}

static rbnode_s *rbnode_new(const void *data, size_t size) {
    rbnode_s *self = calloc(1, sizeof(*self) + size);
    if (self) {
        memcpy(self->data, data, size);
    }
    return self;
}

static rbtree_s *rbtree_new(compare_f cmp) {
    rbtree_s *self = calloc(1, sizeof(*self));
    if (self) {
        self->cmp = cmp;
    }
    return self;
}

static bool rbnode_is_red(rbnode_s *self) {
    if (!self) {
        return false;
    }
    return !self->is_black;
}

static rbnode_s *rbtree_rbnode_insert(
    rbtree_s self[static 1],
    rbnode_s *node,
    const void *data,
    size_t size
) {
    if (node == nullptr) {
        return rbnode_new(data, size);
    }

    if (rbnode_is_red(node->kids[0]) && rbnode_is_red(node->kids[1])) {
        rbnode_color_flip(node);
    }

    int diff = self->cmp(data, node->data);
    if (diff != 0) {
        node->kids[diff < 0] = rbtree_rbnode_insert(
                self, node->kids[diff < 0], data, size);
    }

    if (rbnode_is_red(node->kids[1]) && !rbnode_is_red(node->kids[0])) {
        node = rbnode_rotate(node, false);
    }
    if (rbnode_is_red(node->kids[0]) && rbnode_is_red(node->kids[0]->kids[0])) {
        node = rbnode_rotate(node, true);
    }

    return node;
}

static void rbtree_insert(
    rbtree_s self[static 1],
    const void *data,
    size_t size
) {
    self->head = rbtree_rbnode_insert(self, self->head, data, size);
    self->head->is_black = true;
    self->size++;
}
