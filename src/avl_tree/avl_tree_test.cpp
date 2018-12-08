#include "avl_tree.h"
#include <cstdio>


// void avlTreeNodeNilSetHeight() {
//   // if (AvlTreeNode::NIL == nullptr) {
//   // *(AvlTreeNode **)&(AvlTreeNode::NIL) = new AvlTreeNode;
//     ((AvlTreeNode *)AvlTreeNode::NIL)->height = 0;
//   // }
// }

int main(void) {
  AvlTreeNode::InitNIL();
  std::printf("%p\n", AvlTreeNode::NIL);
  AvlTreeNode *t = new AvlTreeNode(0);
  t = t->Insert(17);
  t = t->Insert(29);
  t = t->Insert(13);
  t = t->Insert(44);
  t = t->Insert(15);
  t = t->Insert(69);
  t = t->Insert(27);
  t = t->Insert(18);
  t = t->Insert(59);
  t = t->Insert(23);
  t = t->Insert(77);
  t = t->Insert(49);
  t = t->Insert(11);
  t = t->Insert(36);
  t = t->Insert(40);
  t = t->Insert(61);
  t->Show();
}
