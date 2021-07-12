#!/bin/bash
# $1 - profile file to analyze
# $2 - wsize
# $3 - shift size

UTILS_ROOT=~/locality/analyze/metric

WINDOW=$2
SIZE=$3

if [[ -z $1 ]]
then
	echo file name not specified
	exit 1
fi

if [[ -z $WINDOW ]]
then
	WINDOW=128
fi

if [[ -z $SIZE ]]
then
	SIZE=6
fi

#echo "running with $WINDOW $SIZE"

OUT=`mktemp`

$UTILS_ROOT/analyze.py -v $1 -w $WINDOW -s $SIZE > $OUT
$UTILS_ROOT/avg.sh $OUT
 

