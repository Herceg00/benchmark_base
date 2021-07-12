#! /usr/bin/env python

import sys
import re
import operator
import math

def Avg(vals):
	return (sum(vals), sum(map(operator.mul, vals, range(len(vals)))))

def StdDev(l, mean, count):
	s = 0
	
	for i, occ in enumerate(l):
		tmp = (i - mean)
		s += 1.0 * tmp * tmp * occ

	return math.sqrt(s / count)


for line in sys.stdin:
	if "locinfo" in line and "histo" in line:
		arr = re.search("val :(.*);", line).group(1)
		vals = [int(x) for x in arr.split(" ") if x.isdigit()]

		name = line.split(";")[1].split(":")[1]

		if "_" in name:
			while True:
				if name[-1] != "_":
					name = name[:-1]
				else:
					name = name[:-1]
					break
		if "_" in name:
			while True:
				if name[-1] != "_":
					name = name[:-1]
				else:
					break
		else:
			name += "_"

		name += line.split(";")[2].split(":")[1].strip()

		print
		print
		print name
		print
		print vals

		count, val_sum = Avg(vals)

		mean = float(val_sum) / count

		s = StdDev(vals, mean, count)

		ints = [ \
			(float("-inf"), mean - 3*s, "-inf .. -3s"),
			(mean - 3*s,    mean - 2*s, " -3s .. -2s"),
			(mean - 2*s,    mean - 1*s, " -2s .. -1s"),
			(mean - 1*s,    mean      , " -1s ..  0s"),
			(mean      ,    mean + 1*s, "  0s ..  1s"),
			(mean + 1*s,    mean + 2*s, "  1s ..  2s"),
			(mean + 2*s,    mean + 3*s, "  2s ..  3s"),
			(mean + 3*s, float("inf") , "  3s .. inf")]

#		print
#		for interval in ints:
#			print filter(lambda x: interval[0] <= x[0] < interval[1], enumerate(vals))

		print
		print name, "count: ", count
		print name, "val_sum: ", val_sum
		print name, "mean: ", mean
		print name, "std_dev: ", s
		print

		for interval in ints:
			p_count = reduce(lambda acc,x: acc + x[1], filter(lambda x: interval[0] < x[0] < interval[1], enumerate(vals)), 0)

			print "%s : %5.f .. %5.f :  %10d   %5.1f%%" % (interval[2], interval[0], interval[1], p_count, 100.0 * p_count / count)
