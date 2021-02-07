#!/bin/bash
PERF_PATTERN_BW="avg_bw"
PERF_PATTERN_TIME="avg_time"
PERF_PATTERN_FLOPS="avg_flops"
PROG_NAME=$1
LAST_MODE=$2
LENGTH=$3
ELEMS=$4
while [ $# -gt 0 ]; do
  case "$1" in
    --prog=*)
      PROG_NAME="${1#*=}"
      ;;
    --mode=*)
      LAST_MODE="${1#*=}"
      ;;
    --length=*)
      LENGTH="${1#*=}"
      ;;
    --radius=*)
      ELEMS="${1#*=}"
      ;;
    *)
      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

cd ./$PROG_NAME || return
rm tmp*
#rm results.txt
for ((i=1; i < $LAST_MODE; i++))
do
rm -r bin
make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$i
./bin/$PROG_NAME""_np_STD > tmp_file_mode$i''.txt
search_result=$(grep -R "$PERF_PATTERN_BW" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "mode $i $perf" >> results.txt
search_result=$(grep -R "$PERF_PATTERN_TIME" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "mode $i $perf" >> results.txt
search_result=$(grep -R "$PERF_PATTERN_FLOPS" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "mode $i $perf " >> results.txt
echo "" >> results.txt
done