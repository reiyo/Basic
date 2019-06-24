# refer to https://docs.python.org/zh-tw/3/library/heapq.html
# refer to https://stackoverflow.com/questions/11989178

import heapq

# mostly, heappush() and heappop() are enough;
# more efficient methods can be found at https://docs.python.org/zh-tw/3/library/heapq.html

##########################

h = []
heapq.heappush(h, 20)
heapq.heappush(h, 10)
heapq.heappush(h, 30)

print (h[0]) # 10
val = heapq.heappop(h)
print (val) # 10

##########################

class MyObject(object):
    def __init__(self, val):
        self.val = val
    def __eq__(self, other):
        return self.val == other.val
    def __lt__(self, other): # replace < with > for max heap 
        return self.val < other.val

h = []
heapq.heappush(h, MyObject(40))
heapq.heappush(h, MyObject(70))
heapq.heappush(h, MyObject(30))
heapq.heappush(h, MyObject(60))
obj = heapq.heappop(h)
print (obj.val) # 30

