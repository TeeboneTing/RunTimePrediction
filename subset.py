#!/usr/bin/env python
import sys
import random

f = open(sys.argv[1],"r")
lines = int(sys.argv[2])
fo1 = open(sys.argv[3],"w")
fo2 = open(sys.argv[4],"w")
array = []

for log in f:
	array.append(log);

random.seed()
random.shuffle(array)

for i in range(lines):
	fo1.write(array[i])

for l in array[lines:]:
	fo2.write(l)