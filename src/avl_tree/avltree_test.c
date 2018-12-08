#include "avltree.h"
#include <stdio.h>

void init(void) {
  *(avl_tree_node_t **)&AVL_TREE_NODE_NIL = malloc(sizeof *AVL_TREE_NODE_NIL);
  ((avl_tree_node_t *)AVL_TREE_NODE_NIL) ->height = 0;
}
inline static int max(int x, int y) { return x > y ? x : y; }

inline static avl_tree_node_t *avl_tree_node__rotate_right(avl_tree_node_t *o) {
  avl_tree_node_t *pivot = o->left;
  o->left = pivot->right;
  pivot->right = o;
  o->height = 1 + max(o->left->height, o->right->height);
  pivot->height = 1 + max(pivot->left->height, pivot->right->height);
  return pivot;
}

inline static avl_tree_node_t *avl_tree_node__ll_balance(avl_tree_node_t *o) {
  return avl_tree_node__rotate_right(o);
}

int main(void) {
  init();

  avl_tree_node_t *t = malloc(sizeof *t);
  /* avl_tree_node_init(t, 5); */
  /* avl_tree_node_t *t_left = malloc(sizeof *t_left); */
  /* avl_tree_node_init(t_left, 3); */
  /* avl_tree_node_t *t_left_left = malloc(sizeof *t_left_left); */
  /* avl_tree_node_init(t_left_left, 2); */
  /* t->left = t_left; */
  /* t->left->left = t_left_left; */
  /* avl_tree_node_show(t); */
  /* printf("\n"); */
  /* t = avl_tree_node__ll_balance(t); */
  /* avl_tree_node_show(t); */
  avl_tree_node_init(t, 0);
  t = avl_tree_node_insert(t, 17);
  t = avl_tree_node_insert(t, 29);
  t = avl_tree_node_insert(t, 13);
  t = avl_tree_node_insert(t, 44);
  t = avl_tree_node_insert(t, 15);
  t = avl_tree_node_insert(t, 69);
  t = avl_tree_node_insert(t, 27);
  t = avl_tree_node_insert(t, 18);
  t = avl_tree_node_insert(t, 59);
  t = avl_tree_node_insert(t, 23);
  t = avl_tree_node_insert(t, 77);
  t = avl_tree_node_insert(t, 49);
  t = avl_tree_node_insert(t, 11);
  t = avl_tree_node_insert(t, 36);
  t = avl_tree_node_insert(t, 40);
  t = avl_tree_node_insert(t, 61);
  t = avl_tree_node_delete(t, 59);
  avl_tree_node_show(t);
}
