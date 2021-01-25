#!/usr/bin/env python

phys = open(options.phys_fname).readlines()

phys = map(lambda x: int(x), phys)

if len(virt) != len(phys):
	print "size does not match"
	exit(1)

l = len(virt)

match = []
diff = []
achtung = 0
PAGE = 4096

for i in xrange(1, l):

	d_virt = virt[i] % PAGE
	d_phys = phys[i] % PAGE

	if d_phys == d_virt:
		match.append(d_virt)
	
	elif d_phys != d_virt:
		diff.append(d_virt)

l_match = len(match)
l_diff = len(diff)

print "match", l_match
if l_match != 0: 
	print "m_avg %.2f" % (sum(match) * 1.0 / l_match)
	print "m_med ", match[l_match / 2]

print
print "diff", l_diff
if l_diff != 0: 
	print "d_avg %.2f" % (sum(diff) * 1.0 / l_diff)
	print "d_med ", diff[l_diff / 2]

print
print "achtung", achtung

print
print "total", l

