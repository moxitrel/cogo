class _NilAvlTreeNode:
    """Class descriptions."""

    def __init__(self):
        self.height = 0


Nil = _NilAvlTreeNode()


class AvlTreeNode:
    """Class descriptions.

    AvlTree:
        | Nil:
          {
              .Height: 0
              .insert ^x: Node x
          }
        | Node ^value ^leftTree ^rightTree:
          {
              .Height:
                  1 + (max .LeftTree.Height .RightTree.Height)
              .insert ^value:
                  v: avlTree ~ Nil => Node value Nil Nil
                   | value >  avlTree.value => avlTree.RightTree .insert value
                   | value <= avlTree.value => avlTree.LeftTree  .insert value
                  v.Height = 1 + (max avlTree.LeftTree.Height avlTree.Right.Height)
                  | rightTree.Height - leftTree.Height >  1 =>
                      | rightTree.BalanceFactor >  0 =>
                          v.RRBalance
                      | rightTree.BalanceFactor <= 0 =>
                          v.RLBalance
                  | rightTree.Height - leftTree.Height < -1 =>
              .Delete ^value :=
                  | value < self.value => Node self.value (leftTree.Delete value) rightTree
                  | value > self.value => Node self.value leftTree (rightTree.Delete value)
                  | value ~ self.value =>
                      | leftTree, rightTree ~ Nil, Nil => Nil
                      | rightTree.Height > leftTree.Height =>
                          v := rotateLeft
                          Node v.value (v.leftTree.Delete value) v.rightTree
                      | rightTree.Height <= leftTree.Height =>
                          v := rotateRight
                          Node v.value v.leftTree (v.rightTree.Delete value)
          }
    """

    def __init__(self, value, left=Nil, right=Nil):
        self.value = value
        self.left = left
        self.right = right
        self.height = 1

    def delete(self, value):
        """Function description."""
        if value < self.value:
            self.left = self.left.delete(value)
            return self
        elif value > self.value:
            self.right = self.right.delete(value)
            return self
        else:
            if self.left is Nil and self.right is Nil:
                return Nil
            elif self._balance_factor() > 0:
                v = self._rotate_left()
                v.left = v.left.delete(value)
                return v
            else:
                v = self._rotate_right()
                v.right = v.right.delete(value)
                return v

    def show(self):
        """Function description."""
        if self.left is not Nil:
            print(self.value, "->", self.left.value)
            self.left.show()
        if self.right is not Nil:
            print(self.value, "->", self.right.value)
            self.right.show()

    def _balance_factor(self):
        """Function description."""
        return self.right.height - self.left.height

    def insert(self, value):
        if value > self.value:
            self.right = self.right.insert(value)
            self.height = 1 + max(self.left.height, self.right.height)
            if self._balance_factor() == 2:
                if self.right._balance_factor() == 1:
                    # RR
                    return self.rr_balance()
                else:
                    # RL
                    return self.rl_balance()
            else:
                return self
        else:
            self.left = self.left.insert(value)
            self.height = 1 + max(self.left.height, self.right.height)
            if self._balance_factor() == -2:
                if self.left._balance_factor() == -1:
                    return self.ll_balance()
                else:
                    return self.lr_balance()
            else:
                return self

    def ll_balance(self):
        """Function description."""
        return self._rotate_right()

    def lr_balance(self):
        """Function description."""
        self.left = self.left._rotate_left()
        return self._rotate_right()

    def rl_balance(self):
        """Function description."""
        self.right = self.right._rotate_right()
        return self._rotate_left()

    def rr_balance(self):
        """Function description."""
        return self._rotate_left()

    def _rotate_right(self):
        """Function description."""
        pivot = self.left
        self.left = pivot.right
        pivot.right = self

        self.height = 1 + max(self.left.height, self.right.height)
        pivot.height = 1 + max(pivot.left.height, pivot.right.height)
        return pivot

    def _rotate_left(self):
        """Function description."""
        pivot = self.right
        self.right = pivot.left
        pivot.left = self

        self.height = 1 + max(self.left.height, self.right.height)
        pivot.height = 1 + max(pivot.left.height, pivot.right.height)
        return pivot


def insert(self, value):
    return AvlTreeNode(value)


_NilAvlTreeNode.insert = insert
