#!/usr/bin/env bash

for dir in input/*
do
	short_dir=`basename $dir`

	if [ ! -d "$dir" ]
	then
		continue
	fi
	

	if [ ! -d "output/$short_dir" ]
	then
		mkdir output/$short_dir
	fi

	for file in $dir/*
	do
		echo processing $file
		short_file=`basename $file`

		if [ -f output/$short_dir/$short_file.char.csv ]; then
			echo Filex exists - skipping
			continue
		fi
	
		cd bin && ./out ../$file &> ../output/$short_dir/$short_file.out.log
	
		mv characteristics.csv ../output/$short_dir/$short_file.char.csv

		cd -
		echo $file done		
	done
done
