import sys

from collections import defaultdict

data = []

with open(sys.argv[1]) as f:
	
	for line in f.readlines():
		data.append(int(line))

K = 100

for i in range(0, len(data), K):
	print sum(data[i:i+K])
