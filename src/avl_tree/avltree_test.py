import avltree


class TestAvlTree:
    """Class descriptions."""

    def test_ll(self):
        """Function description."""
        t = avltree.AvlTreeNode(5)
        t.left = avltree.AvlTreeNode(3)
        t.left.left = avltree.AvlTreeNode(2)
        t.show()
        print()
        t.ll_balance().show()

    def test_rr(self):
        """Function description."""
        t = avltree.AvlTreeNode(3)
        t.right = avltree.AvlTreeNode(5)
        t.right.right = avltree.AvlTreeNode(7)
        t.show()
        print()
        t.rr_balance().show()

    def test_lr(self):
        """Function description."""
        t = avltree.AvlTreeNode(5)
        t.left = avltree.AvlTreeNode(3)
        t.left.right = avltree.AvlTreeNode(4)
        t.show()
        print()
        t.lr_balance().show()

    def test_rl(self):
        """Function description."""
        t = avltree.AvlTreeNode(3)
        t.right = avltree.AvlTreeNode(5)
        t.right.left = avltree.AvlTreeNode(4)
        t.show()
        print()
        t.rl_balance().show()

    def test_insert(self):
        t = avltree.AvlTreeNode(0)
        t = t.insert(17)
        t = t.insert(29)
        t = t.insert(13)
        t = t.insert(44)
        t = t.insert(15)
        t = t.insert(69)
        t = t.insert(27)
        t = t.insert(18)
        t = t.insert(59)
        t = t.insert(23)
        t = t.insert(77)
        t = t.insert(49)
        t = t.insert(11)
        t = t.insert(36)
        t = t.insert(40)
        t = t.insert(61)
        # t = t.delete(59)
        t.show()
