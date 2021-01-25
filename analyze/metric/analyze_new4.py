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

(options, args) = parser.parse_args()

	
if not options.virt_fname:
    parser.error('virt_fname not given')

if not options.window_size:
    parser.error('window_size not given')

if not options.shift:
    parser.error('shift not given')

virt = open(options.virt_fname).readlines()

virt = map(lambda x: int(x), virt)

window_size = int(options.window_size)
shift = int(options.shift)

# returns total pages count and distribution of pages with windows size and shift
def GetPages(virt, window_size, shift):
	pages = map(lambda x: x >> shift, virt)

	if window_size > len(pages):
		print "bad windows size"
		return

	res = []

	for i in xrange(0, len(pages)-window_size):

		while
		vals = virt[i:i+window_size]

		res.append(len(set(vals)))


	return (len(set(pages)), res)

(count, rects) = GetPages(virt, window_size, shift)

print count

for r in rects:
	print r

# for (( c=500; c<=10000; c+=100 )); do echo -n "$c: " &&head -n $c ../input/matmult/mult.virt.log.3 | tail -n 500 | while read line; do echo $line / 64 | bc; done | sort | uniq | wc -l; done
