#include "avltree.h"
#include <stdio.h>

inline static int avl_tree_node__balance_factor(const avl_tree_node_t *o) {
  assert(o);
  return o->right->height - o->left->height;
}

inline static int max(int x, int y) { return x > y ? x : y; }

void avl_tree_node_show(const avl_tree_node_t *o) {
  assert(o);
  /* printf("AVL_TREE_NODE_NIL: %p\n", AVL_TREE_NODE_NIL); */
  /* printf("o: %p\n", o); */
  /* printf("o->left: %p\n", o->left); */
  /* printf("o->right: %p\n", o->right); */
  if (o->left != AVL_TREE_NODE_NIL) {
    printf("%d -> %d\n", o->value, o->left->value);
    avl_tree_node_show(o->left);
  }
  if (o->right != AVL_TREE_NODE_NIL) {
    printf("%d -> %d\n", o->value, o->right->value);
    avl_tree_node_show(o->right);
  }
}

inline static avl_tree_node_t *avl_tree_node__rotate_right(avl_tree_node_t *o) {
  avl_tree_node_t *pivot = o->left;
  o->left = pivot->right;
  pivot->right = o;
  o->height = 1 + max(o->left->height, o->right->height);
  pivot->height = 1 + max(pivot->left->height, pivot->right->height);
  return pivot;
}

inline static avl_tree_node_t *avl_tree_node__rotate_left(avl_tree_node_t *o) {
  avl_tree_node_t *pivot = o->right;
  o->right = pivot->left;
  pivot->left = o;
  o->height = 1 + max(o->left->height, o->right->height);
  pivot->height = 1 + max(pivot->left->height, pivot->right->height);
  return pivot;
}

inline static avl_tree_node_t *avl_tree_node__ll_balance(avl_tree_node_t *o) {
  return avl_tree_node__rotate_right(o);
}

inline static avl_tree_node_t *avl_tree_node__rr_balance(avl_tree_node_t *o) {
  return avl_tree_node__rotate_left(o);
}

inline static avl_tree_node_t *avl_tree_node__lr_balance(avl_tree_node_t *o) {
  o->left = avl_tree_node__rotate_left(o->left);
  return avl_tree_node__rotate_right(o);
}

inline static avl_tree_node_t *avl_tree_node__rl_balance(avl_tree_node_t *o) {
  o->right = avl_tree_node__rotate_right(o->right);
  return avl_tree_node__rotate_left(o);
}

avl_tree_node_t *avl_tree_node_insert(avl_tree_node_t *o, value_t value) {
  if (o == AVL_TREE_NODE_NIL) {
    avl_tree_node_t *v = malloc(sizeof *v);
    assert(v);
    avl_tree_node_init(v, value);
    return v;
  } else {
    if (value > o->value) {
      o->right = avl_tree_node_insert(o->right, value);
      o->height = 1 + max(o->left->height, o->right->height);
      if (avl_tree_node__balance_factor(o) == 2) {
        if (avl_tree_node__balance_factor(o->right) == 1) {
          return avl_tree_node__rr_balance(o);
        } else {
          return avl_tree_node__rl_balance(o);
        }
      } else {
        return o;
      }
    } else {
      o->left = avl_tree_node_insert(o->left, value);
      o->height = 1 + max(o->left->height, o->right->height);
      if (avl_tree_node__balance_factor(o) == -2) {
        if (avl_tree_node__balance_factor(o->left) == -1) {
          return avl_tree_node__ll_balance(o);
        } else {
          return avl_tree_node__lr_balance(o);
        }
      } else {
        return o;
      }
    }
  }
}

avl_tree_node_t *avl_tree_node_delete(avl_tree_node_t *o, value_t value)
{
  avl_tree_node_t *v = o;

  if (value < o->value) {
    o->left = avl_tree_node_delete(o->left, value);
  } else if (value > o->value) {
    o->right = avl_tree_node_delete(o->right, value);
  } else {
    if (o->left == AVL_TREE_NODE_NIL && o->right == AVL_TREE_NODE_NIL) {
      v = AVL_TREE_NODE_NIL;
    } else if (avl_tree_node__balance_factor(o) > 0) {
      v = avl_tree_node__rotate_left(o);
      v->left = avl_tree_node_delete(v->left, value);
    } else {
      v = avl_tree_node__rotate_right(o);
      v->right = avl_tree_node_delete(v->right, value);
    }
  }

  return v;
}
