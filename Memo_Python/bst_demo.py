# refer to https://stackoverflow.com/questions/5444394

class Node:
    def __init__(self, val):
        self.l_child = None
        self.r_child = None
        self.data = val


def binary_insert(root, node):
    if root is None:
        root = node
    else:
        if root.data > node.data:
            if root.l_child is None:
                root.l_child = node
            else:
                binary_insert(root.l_child, node)
        else:
            if root.r_child is None:
                root.r_child = node
            else:
                binary_insert(root.r_child, node)


# inorder traversal for a Binary Search Tree will result in the numbers in the ascending order
def in_order_print(root):
    if not root:
        return
    in_order_print(root.l_child)
    print (root.data)
    in_order_print(root.r_child)


if __name__ == '__main__':
    root = None

    while 1:
        in_str = input("Input a number for BST: ")

        if not in_str:
            break

        value = float(in_str)

        if not root:
            root = Node(value)
        else:
            newNode = Node(value)
            binary_insert(root, newNode)

        in_order_print(root)


