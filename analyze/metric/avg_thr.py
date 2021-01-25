#! /usr/bin/env python

import sys

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-v", "--virt", dest="virt_fname",
                  help="virtual addresses log")

parser.add_option("-t", "--threshold", dest="thr",
                  help="threshold", default=0)

(options, args) = parser.parse_args()

if not options.virt_fname:
    parser.error('profile file name not given')

virt = open(options.virt_fname).readlines()

virt = map(lambda x: int(x), virt)

orig_len = len(virt)

before = len(virt)

virt = filter(lambda x: x > int(options.thr), virt)

after = len(virt)

#print "before: ", before
#print "after: ", after
#print "dropped: ", 100.0 - 100.0 * after / before

print 1.0 * sum(virt) / orig_len #len(virt)