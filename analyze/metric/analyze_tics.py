#! /usr/bin/env python

import sys
from collections import defaultdict

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-v", "--virt", dest="virt_fname",
                  help="virtual addresses log")

parser.add_option("-w", "--wsize", dest="window_size",
                  help="size for scan window", default=1024)

parser.add_option("-s", "--shift", dest="shift",
                  help="power of shift( / 2 ^ sh)", default=12)

parser.add_option("-o", "--ofile", dest="out_file",
                  help="output file prefx")

(options, args) = parser.parse_args()

	
if not options.virt_fname:
    parser.error('virt_fname not given')

if not options.window_size:
    parser.error('window_size not given')

if not options.shift:
    parser.error('shift not given')

if not options.out_file:
    parser.error('out_file not given')

virt = open(options.virt_fname).readlines()

virt = map(lambda x: int(x), virt)

window_size = int(options.window_size)
shift = int(options.shift)

base_name = options.out_file + ".w%05d" % window_size + ".s%d" % shift

out = base_name + ".tics"

ticks_out = open(out, "w")

K1 = 320
K2 = 5
K3 = 1

def Log(str):
	return
	print str

def AnalyzeWindow(addresses, size):
	addresses = sorted(addresses)

	last_page = -1
	pages_count = 0

	ticks = 0

	for addr in addresses:
		page = addr / size
		line = addr % size

		Log((addr, page, line, last_page))

		if page == last_page:
			if line in lines_used:
				ticks += K3
				Log("line %d - register" % line)
			
			else:
				lines_used.add(line)
				ticks += K2
				Log("cache")

		else:
			last_page = page
			lines_used = set()
			ticks += K1
			pages_count += 1
			Log("new_page")

	Log("")
	Log(ticks)
	Log("")

	return (pages_count, ticks)

res = []

for start in range(len(virt) - window_size):
	if start % 100 == 0:
		print start

	res.append(AnalyzeWindow(virt[start:window_size + start], 1 << shift))

for r in res:
	print >> ticks_out, r[1]

# for (( c=500; c<=10000; c+=100 )); do echo -n "$c: " &&head -n $c ../input/matmult/mult.virt.log.3 | tail -n 500 | while read line; do echo $line / 64 | bc; done | sort | uniq | wc -l; done
