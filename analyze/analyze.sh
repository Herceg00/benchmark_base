#!/bin/bash

# $1 - directory to program profile

#windows="128 256 512 1024 2048 4096 8192 16384 32768 65536"
windows="512"
rect_sizes="6"

if [ -z "$1" ]
then
	echo "input directory not given"
	exit 1
fi

if [ ! -d "$1" ]
then
	echo "input directory does not exist : $1"
	exit 1
fi

out_dir=output/`basename $1`

mkdir -p $out_dir/profile_pics
mkdir -p $out_dir/metric
mkdir -p $out_dir/metric_pics

for f in $1*
do
	fout=$out_dir/profile_pics/`basename $f`.png

	if [ ! -f "$fout" ]
	then
		echo "plotting $fout"

		metric/plot_simple.sh $f dots > $fout
	else
		echo "$fout exists - skipping"
	fi
done

echo "creating metrics"

for window in $windows
do
	for rect_size in $rect_sizes
	do
		for f in $1*virt*
		do
			fout=$out_dir/metric/$window/$rect_size/`basename $f`.rects

			if [ ! -f "$fout" ]
			then
				echo "creatin $fout"

				mkdir -p $out_dir/metric/$window/$rect_size
				metric/analyze.py -v $f -w $window -s $rect_size | tail -n +2 | cat > $fout

				# first line - max metric
			else
				echo "$fout exists - skipping"
			fi				
		done
	done
done

exit 0

echo "plotting metrics"

for window in $windows
do
	for rect_size in $rect_sizes
	do
		for f in $1*virt*
		do
			data=$out_dir/metric/$window/$rect_size/`basename $f`.rects
			fout=$out_dir/metric_pics/$window/$rect_size/`basename $f`.png

			if [ ! -f "$data" ]
			then
				echo "some error occured, $fout not exists"
				exit 1 
			else
				if [ ! -f "$fout" ]
				then
					echo "plotting $fout"

					mkdir -p $out_dir/metric_pics/$window/$rect_size
					metric/plot_simple.sh $data lines > $fout
				else
					echo "$fout exists - skipping"
				fi	
			fi				
		done
	done
done
