#!/bin/bash

for f in $@
do
	./analyze.sh $f/
done

