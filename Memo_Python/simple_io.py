#!/usr/bin/python

############# read input arguments #############

import sys
print('Get argument sys.argv[0]: ', sys.argv[0])
print()


############# read from standard inputs #############

str1 = input('input your integer: ')
str2 = input('input your float number: ')

int_num = int(str1)
float_num = float(str2)

print('Get', int_num, 'and', float_num)
print()


############ formatted print ############

print('Get %d %.2f %s' % (int_num, float_num, str1))

