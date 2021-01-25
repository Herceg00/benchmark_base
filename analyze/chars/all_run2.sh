#!/usr/bin/env bash

K=1
N=2
MODE=0

for file in input/linpack*
do
	echo processing $file
	short_file=`basename $file`
	if [ -f output/$short_file.char.csv ]; then
		echo File exists - skipping
		continue
	fi

	cd bin && ./out ../$file $K $N $MODE &> ../output/$short_file.out.log

	mv characteristics.csv ../output/$short_file.char.csv
	cd -
	echo $file done		
done
