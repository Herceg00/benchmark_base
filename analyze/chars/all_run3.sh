#!/usr/bin/env bash

K=1
N=2
MODE=0

MAX_SIZE=10000000

for file in input/united/*
do
	echo processing $file
	file_size=$(wc -l $file | awk '{print $1}');
	if [ $(( $file_size )) -gt $MAX_SIZE ]; then 
		echo 'Skipping[' $file_size ']'
		continue
	else echo 'Nooo' $file_size
	fi
	echo '___'
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
