# Queue can be achieved by several ways.

##########################
# 1: use deque
# refer to https://leetcode.com/problems/binary-tree-level-order-traversal/discuss/33780
from collections import deque

q = deque()
q.append('a')
q.append('b')
q.append('c')

while q:
    data = q.popleft()
    print (data)

##########################
# 2: use list (seems not a good idea)
# append() + pop(0) can work; however, pop(0) takes O(n) time; refer to https://stackoverflow.com/questions/195625
# insert(0, ) + pop() can also work; however, insert(0, ) may take O(n) time; refer to https://stackoverflow.com/questions/27073596

q = []
q.append('a')
q.append('b')
q.append('c')

while q:
    data = q.pop(0)
    print (data)

q = []
q.insert(0, 'a')
q.insert(0, 'b')
q.insert(0, 'c')

while q:
    data = q.pop()
    print (data)

##########################
# 3. use queue (Python 2: Queue, Python 3: queue)
import queue

q = queue.Queue()
q.put('a')
q.put('b')
q.put('c')

while not q.empty():
    data = q.get()
    print (data)

