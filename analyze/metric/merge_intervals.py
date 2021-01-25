#! /usr/bin/env python

import sys
from collections import defaultdict

virt = open(sys.argv[1]).readlines()
virt = map(int, virt)

v_min = int(virt[0])
v_max = int(virt[0])

for val in virt:
	if val > v_max:
		v_max = val

	if val < v_min:
		v_min = val

mid = v_min + (v_max - v_min) / 2

mid_min = v_max
mid_max = v_max

for val in virt:
	if val > mid and val - mid < mid_max:
		mid_max = val - mid

	if val < mid and mid - val < mid_min:
		mid_min = mid - val


#print v_min, v_max
#print mid

#print mid_min, mid_max

THR = 1000

#exit(0)

if mid_max + mid_min > 2 * THR:
	for val in virt:
		if val > mid:
			print val - mid_max - mid_min + THR - v_min
		else:
			print val - v_min
else:
	for val in virt:
		print val - mid_max - mid_min + THR - v_min

