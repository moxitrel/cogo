#include "avl_tree.h"
#include <cstdio>

const AvlTreeNode *const AvlTreeNode::NIL = new AvlTreeNode;

inline int AvlTreeNode::balanceFactor() {
  return this->right->height - this->left->height;
}

template <typename T>
inline T max(T x, T y) {
  return x > y ? x : y;
}

void AvlTreeNode::Show() {
  if (this->left != AvlTreeNode::NIL) {
    std::printf("%d -> %d\n", this->value, this->left->value);
    this->left->Show();
  }
  if (this->right != AvlTreeNode::NIL) {
    std::printf("%d -> %d\n", this->value, this->right->value);
    this->right->Show();
  }
}

AvlTreeNode *AvlTreeNode::rotateRight() {
  AvlTreeNode *pivot = this->left;
  this->left         = pivot->right;
  pivot->right       = this;
  this->height       = 1 + max<int>(this->left->height, this->right->height);
  pivot->height      = 1 + max<int>(pivot->left->height, pivot->right->height);
  return pivot;
}

AvlTreeNode *AvlTreeNode::rotateLeft() {
  AvlTreeNode *pivot = this->right;
  this->right        = pivot->left;
  pivot->left        = this;
  this->height       = 1 + max<int>(this->left->height, this->right->height);
  pivot->height      = 1 + max<int>(pivot->left->height, pivot->right->height);
  return pivot;
}

inline AvlTreeNode *AvlTreeNode::balanceLL() {
  return this->rotateRight();
}

inline AvlTreeNode *AvlTreeNode::balanceRR() {
  return this->rotateLeft();
}

inline AvlTreeNode *AvlTreeNode::balanceLR() {
  this->left = this->left->rotateLeft();
  return this->rotateRight();
}

inline AvlTreeNode *AvlTreeNode::balanceRL() {
  this->right = this->right->rotateRight();
  return this->rotateLeft();
}

AvlTreeNode *AvlTreeNode::Insert(int value) {
  if (this == AvlTreeNode::NIL) {
    return new AvlTreeNode(value);
  } else {
    if (value > this->value) {
      this->right  = this->right->Insert(value);
      this->height = 1 + max<int>(this->left->height, this->right->height);
      if (this->balanceFactor() == 2) {
        if (this->right->balanceFactor() == 1) {
          return this->balanceRR();
        } else {
          return this->balanceRL();
        }
      } else {
        return this;
      }
    } else {
      this->left   = this->left->Insert(value);
      this->height = 1 + max<int>(this->left->height, this->right->height);
      if (this->balanceFactor() == -2) {
        if (this->left->balanceFactor() == -1) {
          return this->balanceLL();
        } else {
          return this->balanceLR();
        }
      } else {
        return this;
      }
    }
  }
}

AvlTreeNode *AvlTreeNode::Delete(int value) {
  return nullptr;
}
