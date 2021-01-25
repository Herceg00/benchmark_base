#! /usr/bin/env python

import sys
from collections import deque
from optparse import OptionParser

parser = OptionParser()

parser.add_option("-v", "--virt", dest="virt_fname",
                  help="virtual addresses log")

parser.add_option("-d", "--debug", dest="debug",
                  help="more debug info")

parser.add_option("-p", "--prog", dest="prog",
                  help="progress info")

(options, args) = parser.parse_args()

if not options.virt_fname:
    parser.error('virt_fname not given')

virt = open(options.virt_fname).readlines()

virt = map(lambda x: int(x), virt)

reg_size = 8

L1_size = 32 * 1024
L2_size = 256 * 1024
L3_size = 12288 * 1024

cache_line = 64

K_R = 1
K_L1 = 4
K_L2 = 10
K_L3 = 50
K_M = 190

debug = False 

if options.debug == "on":
	debug = True

def Debug(str):
	if not debug:
		return
	print str

# represents fully accotiative cache objects with LRU removing policy, based on queue
class Cache(object):
	def __init__(self, size, penalty, name, cache_line):
		self.size = size / cache_line
		self.penalty = penalty
		self.cache = deque()
		self.check_set = set()
		self.name = name
		self.cache_line = cache_line

	def __contains__(self, addr):
		return (addr / self.cache_line) in self.check_set

	def IsFull(self):
		return len(self.cache) >= self.size

# adds elem to cache or update it position in queue if it presents
	def Add(self, addr):
		cache_addr = addr / self.cache_line
		
		if cache_addr not in self.check_set: 
			self.cache.append(cache_addr) # add new
			self.check_set.add(cache_addr)

			Debug("  " + self.name + "  added " + str(addr))

			if len(self.cache) > self.size: # remove last if full
				rem = self.cache.popleft()
				self.check_set.remove(rem)

				Debug("  " + self.name + "  removed " + str(rem))

		elif cache_addr in self.check_set: # move it to the end
			self.cache.remove(cache_addr) 
			self.cache.append(cache_addr)

			Debug("  " + self.name + "  updated " + str(addr))

reg = Cache(reg_size, K_R, "reg", 1)

L1 = Cache(L1_size, K_L1, "l1", cache_line)
L2 = Cache(L2_size, K_L2, "l2", cache_line)
L3 = Cache(L3_size, K_L3, "l3", cache_line)


#emulate memory flow
def Emul(elems, fake_run, show_progress):
	ticks = 0

	caches = [reg, L1, L2, L3]

	i = 0
	max = len(elems)

	for elem in elems:
		i += 1
		if i % 1000 == 0 and show_progress:
			print "progress %.1f%%" % (100.0 * i / max)

		counted = False

# search it in all caches
		for cache in caches:
			found = elem in cache

			if found:
				if not fake_run:
					ticks += cache.penalty
				counted = True
				Debug(str(elem) + " found in " + cache.name)
				break

# if not found - memory access
		if not counted:
			if not fake_run:
				ticks += K_M
			Debug(str(elem) + " memory access")

# update/add it to all caches
		for cache in caches:
			cache.Add(elem)

		#print L2.size, len(L2.cache), ticks
		if L2.IsFull() and fake_run:
			print "L2 full"
			return i

		Debug("Current ticks:                  " + str(ticks))
		Debug("")

	return ticks

print 1.0 * Emul(virt, False, not options.prog == "off") / len(virt)

#import random

#size = 4096 * 32

#start = random.randint(0, len(virt) - size)

#print "filling cache"
#offset = Emul(virt[start : start + size], True, False)

#print "offset: %d size: %d" % (offset, size)
#print "%d : %d" % (start + offset, start + size - offset)
#print Emul(virt[start + offset: start + size - offset], False, not options.prog == "off")
