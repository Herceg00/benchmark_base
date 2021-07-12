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

K1 = 320
K2 = 5
K3 = 1

def Log(str):
	return
	print str

from collections import defaultdict

def GetSumTicks(virt, window_size, shift):
	parrots = 0

	page = map(lambda x: x >> shift, virt)

	if window_size > len(page):
		print "bad windows size"
		return

	pages = defaultdict(int)
	lines = defaultdict(int)
	res = []

	for i in xrange(0, len(virt)):
		Log("%d %d" % (virt[i], parrots))

		if i >= window_size:
			lines[virt[i-window_size]] -= 1
			pages[page[i-window_size]] -= 1

		if  lines[virt[i]] >  0 and pages[page[i]] == 0:
			print "error detected, something is wrong"
			print lines
			print pages
			exit(1)

		if pages[page[i]] == 0:
			parrots += K1
			Log("new page")
		elif lines[virt[i]] == 0:
			parrots += K2
			Log("new line")
		else:
			parrots += K3
			Log("register")

		lines[virt[i]] += 1
		pages[page[i]] += 1

	return parrots

print GetSumTicks(virt, window_size, shift)

