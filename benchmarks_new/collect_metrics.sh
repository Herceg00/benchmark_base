#!/bin/bash
if [[ ! -d "output" ]]
then
  mkdir ./output/
fi

file_name="./output/event_counters.csv"

COMMON_ARGS="--no_run=false --metrics=true --output=metrics.txt " # TODO threads

while [ $# -gt 0 ]; do
  case "$1" in
    --prog=*)
      PROG_NAME="${1#*=}"
      ;;
    --test_name=*)
      TEST_NAME="${1#*=}"
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
    *)

      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

function remove_spaces() {
    var=$1
    echo "${var//+([[:space:]])/}"
}

function replace_backslash() {
    echo "$1" | tr '/' '*'
}

function parse_events() {
    search_result=$(grep -R "$1" "./$PROG_NAME/metrics.txt")
    search_result=$(replace_backslash $search_result)
    loc_event_name=$(replace_backslash $1)

    parsed_number=`echo $search_result | sed -e "s/"$loc_event_name"//"`

    result=$(remove_spaces $parsed_number)
    echo $result
}

function join_by {
    local IFS="$1"; shift; echo "$*";
}

declare -a event_names=("instructions"
"armv8_pmuv3_0/cpu_cycles/"
"armv8_pmuv3_0/inst_retired/"
"armv8_pmuv3_0/ll_cache_miss/"
"armv8_pmuv3_0/ll_cache/"
"armv8_pmuv3_0/mem_access/"
"armv8_pmuv3_0/remote_access/"
)

# add header
printf "benchmark name," >> $file_name
for event_name in "${event_names[@]}"
do
    printf $event_name"," >> $file_name
done
printf "\n" >> $file_name

# get list of events as a param
list_of_events=$(join_by , "${event_names[@]}")
echo $list_of_events


# collect basic events
echo "--prog=$PROG_NAME --events=$list_of_events --length=$LENGTH --compiler=$COMPILER --radius=$ELEMS --lower_bound=$L_BOUND --higher_bound=$H_BOUND $COMMON_ARGS"
bash make_omp.sh --prog=$PROG_NAME --events=$list_of_events --length=$LENGTH --compiler=$COMPILER --radius=$ELEMS --lower_bound=$L_BOUND --higher_bound=$H_BOUND --threads=$EXP_THREADS $COMMON_ARGS

printf $TEST_NAME"," >> $file_name
for current_name in "${event_names[@]}"
do
  parsed_number=$(parse_events $current_name)
  echo $parsed_number
  printf $parsed_number"," >> $file_name
done
printf "\n" >> $file_name
