#ifndef VAR_TMP_AVLTREE_H
#define VAR_TMP_AVLTREE_H
#include <assert.h>
#include <stdlib.h>

typedef int value_t;

typedef struct avl_tree_node {
  value_t value;
  struct avl_tree_node *left;
  struct avl_tree_node *right;
  unsigned int height;
} avl_tree_node_t;

inline static void avl_tree_node_init(avl_tree_node_t *, value_t);
inline static void avl_tree_node_gc(const avl_tree_node_t **);
void avl_tree_node_show(const avl_tree_node_t *);
avl_tree_node_t *avl_tree_node_insert(avl_tree_node_t *, value_t);
avl_tree_node_t *avl_tree_node_delete(avl_tree_node_t *, value_t);

// NOTE: inited at init()
const avl_tree_node_t *const AVL_TREE_NODE_NIL;

void avl_tree_node_init(avl_tree_node_t *o, value_t value) {
  assert(o);
  o->value = value;
  o->left = AVL_TREE_NODE_NIL;
  o->right = AVL_TREE_NODE_NIL;
  o->height = 1;
}

void avl_tree_node_gc(const avl_tree_node_t **o) {
  assert(o);
  free(*o);
  *o = NULL;
}

#endif // VAR_TMP_AVLTREE_H
