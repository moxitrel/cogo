#ifndef USERS_M2_AVL_TREE_HH
#define USERS_M2_AVL_TREE_HH

class AvlTreeNode {
  int          value;
  AvlTreeNode *left;
  AvlTreeNode *right;
  unsigned int height;

  int          balanceFactor();
  AvlTreeNode *rotateRight();
  AvlTreeNode *rotateLeft();
  AvlTreeNode *balanceLL();
  AvlTreeNode *balanceLR();
  AvlTreeNode *balanceRL();
  AvlTreeNode *balanceRR();

public:
  static const AvlTreeNode *const NIL; // should point to a valid memory
  static void InitNIL() {
    ((AvlTreeNode *)AvlTreeNode::NIL)->height = 0;
  }

  AvlTreeNode()  = default;
  ~AvlTreeNode() = default;
  explicit AvlTreeNode(int value)
      : value(value)
      , left((AvlTreeNode *)AvlTreeNode::NIL)
      , right((AvlTreeNode *)AvlTreeNode::NIL)
      , height(1) {}
  void         Show();
  AvlTreeNode *Insert(int);
  AvlTreeNode *Delete(int);
};

#endif // USERS_M2_AVL_TREE_HH
