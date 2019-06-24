# refer to https://blog.csdn.net/liangguohuan/article/details/7088265

from collections import deque

# len(d): length of the deque d
# d[0]:   the left-end data of the deque d
# d[1]:   the right-end data of the deque d

# extend():     initialize deque; push data from right side
# extendleft(): initialize deque; push data from left side
# append():     append data from the right side
# appendleft(): append data from the left side
# pop():        pop data from the right side
# popleft():    pop data from the left side

##########################

d1 = deque()
d1.extend('abc')
d1.append('d')
print (d1) # deque(['a', 'b', 'c', 'd'])

while len(d1) != 0:
    print ("d1[0] = %s , d1[-1] = %s" % (d1[0], d1[-1]))
    d1.pop()

##########################

d2 = deque()
d2.extendleft('def')
d2.appendleft('g')
print (d2) # deque(['g', 'f', 'e', 'd'])

while len(d2) != 0:
    print ("d2[0] = %s , d2[-1] = %s" % (d2[0], d2[-1]))
    d2.popleft()

##########################

class MyObject(object):
    def __init__(self, val):
        self.val = val

d3 = deque()
d3.append(MyObject(40))
d3.append(MyObject(70))
d3.append(MyObject(30))

while len(d3) != 0:
    print ("d3[0] = %d , d3[-1] = %d" % (d3[0].val, d3[-1].val))
    d3.pop()

