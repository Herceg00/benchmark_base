#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/usr/local/lib64:/usr/lib64
PERF_PATTERN_BW="avg_bw"
PERF_PATTERN_TIME="avg_time"
PERF_PATTERN_FLOPS="avg_flops"
PERF_PATTERN_ROOF="flops/byte"
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
    --compiler=*)
      COMPILER="${1#*=}"
      ;;
    --threads=*)
      EXP_THREADS="${1#*=}"
      ;;
    --lower_bound=*)
      L_BOUND="${1#*=}"
      ;;
    --higher_bound=*)
      H_BOUND="${1#*=}"
      ;;
    --no_run=*)
      NO_RUN="${1#*=}"
      ;;
    *)
      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done
cd ./"$PROG_NAME" || return
for ((i=L_BOUND; i < H_BOUND + 1; i++))
do
rm -r bin
make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$i COMPILER=$COMPILER
if [ $NO_RUN = "false" ]; then
export OMP_NUM_THREADS=$EXP_THREADS
export OMP_PROC_BIND=true
export OMP_PROC_BIND=close
./bin/omp_$PROG_NAME""_np_STD > tmp_file_mode$i''.txt
search_result=$(grep -R "$PERF_PATTERN_BW" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "$perf" >> results.txt
search_result=$(grep -R "$PERF_PATTERN_TIME" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "$perf" >> results.txt
search_result=$(grep -R "$PERF_PATTERN_FLOPS" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "$perf " >> results.txt
search_result=$(grep -R "$PERF_PATTERN_ROOF" tmp_file_mode$i''.txt)
perf=`echo $search_result`
echo "$perf " >> results.txt
echo "" >> results.txt
fi
done


