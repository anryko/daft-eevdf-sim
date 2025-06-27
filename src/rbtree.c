#pragma once

// Red-Black Tree implementation based on Robert Sedgewick paper:
// Left-Leaning Red-Black Trees. Posted here, 2008.

// TODO: Add deletion.
// TODO: Add search.

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static constexpr int left = 0;
static constexpr int right = 1;

typedef int (*compare_f)(const void *p1, const void *p2);

typedef struct rbnode {
    struct rbnode *kids[2];
    bool is_black;
    size_t data_size;
    char data[];
} rbnode_s;

typedef struct rbtree {
    rbnode_s *root;
    size_t size;
    compare_f cmp;
} rbtree_s;

static void rbnode_color_flip(rbnode_s *self) {
    self->is_black = !self->is_black;
    self->kids[left]->is_black = !self->kids[left]->is_black;
    self->kids[right]->is_black = !self->kids[right]->is_black;
}

static rbnode_s *rbnode_rotate(rbnode_s *self, bool to_right) {
    rbnode_s *up = self->kids[!to_right];
    self->kids[!to_right] = up->kids[to_right];
    up->kids[to_right] = self;
    up->is_black = self->is_black;
    self->is_black = false;
    return up;
}

static rbnode_s *rbnode_new(const void *data, size_t data_size) {
    rbnode_s *self = calloc(1, sizeof(*self) + data_size);
    if (self) {
        memcpy(self->data, data, data_size);
        self->data_size = data_size;
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

static inline rbnode_s *rbnode_fix_up(rbnode_s *self) {
    if (rbnode_is_red(self->kids[right]) && !rbnode_is_red(self->kids[left])) {
        self = rbnode_rotate(self, false);
    }
    if (rbnode_is_red(self->kids[left]) && rbnode_is_red(self->kids[left]->kids[left])) {
        self = rbnode_rotate(self, true);
    }

    return self;
}

static rbnode_s *rbtree_rbnode_insert(
    rbtree_s self[static 1],
    rbnode_s *node,
    const void *data,
    size_t size
) {
    if (!node) {
        return rbnode_new(data, size);
    }

    if (rbnode_is_red(node->kids[left]) && rbnode_is_red(node->kids[right])) {
        rbnode_color_flip(node);
    }

    int diff = self->cmp(data, node->data);
    if (diff != 0) {
        node->kids[diff < 0] = rbtree_rbnode_insert(
                self, node->kids[diff < 0], data, size);
    }

    if (rbnode_is_red(node->kids[right]) && !rbnode_is_red(node->kids[left])) {
        node = rbnode_rotate(node, false);
    }
    if (rbnode_is_red(node->kids[left]) && rbnode_is_red(node->kids[left]->kids[left])) {
        node = rbnode_rotate(node, true);
    }

    return rbnode_fix_up(node);
}

static void rbtree_insert(
    rbtree_s self[static 1],
    const void *data,
    size_t data_size
) {
    self->root = rbtree_rbnode_insert(self, self->root, data, data_size);
    self->root->is_black = true;
    ++self->size;
}

static rbnode_s *rbnode_move_red(rbnode_s *self, bool to_right) {
    rbnode_color_flip(self);

    if (rbnode_is_red(self->kids[!to_right]->kids[left])) {
        if (!to_right) {
            self->kids[right] = rbnode_rotate(self->kids[right], true);
        }
        self = rbnode_rotate(self, to_right);
        rbnode_color_flip(self);
    }

    return self;
}

static rbnode_s *rbnode_pop_min(rbnode_s *self, rbnode_s **deleted) {
    if (!self->kids[left]) {
        *deleted = self;
        return nullptr;
    }

    if (!rbnode_is_red(self->kids[left]) && !rbnode_is_red(self->kids[left]->kids[left])) {
        self = rbnode_move_red(self, false);
    }

    self->kids[left] = rbnode_pop_min(self->kids[left], deleted);

    return rbnode_fix_up(self);
}

static void *rbtree_pop_min(rbtree_s *self) {
    rbnode_s *deleted = nullptr;
    self->root = rbnode_pop_min(self->root, &deleted);
    if (self->root) {
        self->root->is_black = true;
    }
    --self->size;

    void *data = malloc(deleted->data_size);
    memcpy(data, deleted->data, deleted->data_size);
    free(deleted);
    return data;
}
