#! /usr/bin/env python

import sys

table = dict()
header = []


for line in sys.stdin:

	if len(line) < 2 or "locinfo" not in line:
		continue

	tokens =  map(lambda x: map(lambda y: y.strip(), x.split(":")), line.split(";"))

	if len(tokens) < 2:
		continue

	name = ""
	type = ""
	value = ""

	for token in tokens[1:]:
		if 'prog' in token[0]:
			name += token[1]

# remove everything after last '_' if it exists
			if "_" in name:
				while True:
					if name[-1] != "_":
						name = name[:-1]
					else:
						name = name[:-1]
						break
			print name
			if "_" in name:
				while True:
					if name[-1] != "_":
						name = name[:-1]
					else:
						break
			else:
				name += "_"
			print name

		if 'prefix' in token[0]:
			name += token[1]
		if 'val' in token[0]:
			value = token[1]
		if 'type' in token[0]:
			type = token[1]

		if type not in header and len(type) > 0:
			header.append(type)

	print "n", name, "t", type, "v", value, "tt", tokens
	
	if name not in table and len(name) > 0:
		table[name] = dict()

	if type in table[name]:
		print " [ * warning * ]: ", name, type, value, "old", table[name][type]
		
		if value != table[name][type]:
			print " [ * error * ]: values mismatch " 
			#sys.exit(-1)
	
	if len(value) > 0:
		table[name][type] = value

#print table

#header = sorted(header)

print
print "csv:"
print

print "name,",
for col in header:
	print "%s," % col,

print

for bench in table:
	print "%s," % bench,

	for col in header:
		if col in table[bench]:
			print "%s," % table[bench][col],
		else :
			print "-1," ,
	print 
