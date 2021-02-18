#!/bin/bash
if [[ ! -d "output" ]]
then
  mkdir ./output/
fi

file_name="./output/event_counters.csv"
file_name_extra="../output/event_counters.csv"



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
    --metrics=*)
      METRICS="${1#*=}"
      ;;
    --events=*)
      EVENTS="${1#*=}"
      ;;
    *)

      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done


function parse_events() {
    search_result=$(grep -R "$current_event" "./$PROG_NAME'/metrics.txt")
    parsed_number=`echo $search_result | sed -e "s/L1-dcache-loads//"`
}


declare -a event_columns=("instructions"
			                   "L1-dcache-loads"
                         )

#print leading row
it="1"
echo $PROG_NAME"," >> file_name
for name in "${event_columns[@]}"
do
    it=$((it+1))
    printf $name"," >> $file_name
done


bash make_omp.sh --prog=$PROG_NAME --events=$name --length=$LENGTH --compiler=$COMPILER --threads="8" --radius=$ELEMS --lower_bound=$L_BOUND --higher_bound=$H_BOUND --no_run=$NO_RUN --metrics="true" --output="metrics.txt"

it="0"
for name in "${event_columns[@]}"
do
  THREADS="$cpus_per_node"
  current_event=name
  printf $parsed_number"," >> $file_name
done
