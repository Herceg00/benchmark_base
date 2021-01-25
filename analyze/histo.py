import sys

from collections import defaultdict

size = int(sys.argv[2])

data = [defaultdict(int) for i in xrange(size)]

min_v = 99999
max_v = 0

n = 0

with open(sys.argv[1]) as f:
	for line in f.readlines():
		n += 1
		for i in xrange(size):
			val = int(line.split(" ")[i])
			if val < min_v:
				min_v = val
			if val > max_v:
				max_v = val
			data[i][val] += 1


for key in range(0, max_v+1):
	for i in xrange(size):
		print data[i][key] * 100.0 / n,
	print
