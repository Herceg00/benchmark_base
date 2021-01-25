import sys

from collections import defaultdict

data = []

with open(sys.argv[1]) as f:
	
	for line in f.readlines():
		data.append(int(line))

size = int(sys.argv[2])
thr = 64

for i in range(size, len(data)):
	s = 0

	for j in range(size):
		dist = abs(data[i-j] - data[i-j-1])
		if dist > thr:
			dist = -thr

		s += dist

	print s
