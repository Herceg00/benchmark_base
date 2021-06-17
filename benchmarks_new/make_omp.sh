#!/bin/bash

if [ ! -d "../external_libraries" ] 
then
    echo "Directory ../external_libraries DOES NOT exists." 
    mkdir ../external_libraries
    cd ../external_libraries
    git clone https://github.com/afanasyev-ilya/VectorGraphLibrary.git
    echo "VectorGraphLibrary has been installed." 
fi

export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/usr/local/lib64:/usr/lib64

while [ $# -gt 0 ]; do
  case "$1" in
    --prog=*)
      PROG_NAME="${1#*=}"
      ;;
    --mode=*)
      MODE="${1#*=}"
      ;;
    --length=*)
      LENGTH="${1#*=}"
      ;;
    --radius=*)
      RADIUS="${1#*=}"
      ;;
    --compiler=*)
      COMPILER="${1#*=}"
      ;;
    --threads=*)
      EXP_THREADS="${1#*=}"
      ;;
    --no_run=*)
      NO_RUN="${1#*=}"
      ;;
    --metrics=*)
      METRICS="${1#*=}"
      ;;
    --events=*)
      EVENTS="${1#*=}"
      ;;
    --output=*)
      OUTPUT="${1#*=}"
      ;;
    --redundant=*)
      REDUNDANT="${1#*=}"
      ;;
    *)

      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

cd ./"$PROG_NAME"

rm -r bin

if [[ $METRICS = "false" ]]; then
    make RADIUS=$RADIUS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=NULL THREADS=$EXP_THREADS
fi

if [[ $METRICS = "true" ]]; then
    make RADIUS=$RADIUS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=METRIC_RUN THREADS=$EXP_THREADS
fi

rm $OUTPUT

if [ $NO_RUN = "false" ]; then
    export OMP_NUM_THREADS=$EXP_THREADS
    export OMP_PROC_BIND=true
    export OMP_PROC_BIND=close
    export LD_LIBRARY_PATH=/opt/intel/oneapi/compiler/2021.1.2/linux/compiler/lib/intel64_lin/:$LD_LIBRARY_PATH
    if [[ $METRICS = "true" ]]; then
        perf stat -o $OUTPUT -a -e $EVENTS ./bin/omp_$PROG_NAME""_np_STD
    fi
    if [[ $METRICS = "false" ]]; then
        ./bin/omp_$PROG_NAME""_np_STD > $OUTPUT
        cp $OUTPUT ./test.txt
fi

fi

cd ../


