#!/bin/bash
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
      ELEMS="${1#*=}"
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
    --force-comp)
      FORCE_COMP=true
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
    # --redundant=*)
    #   REDUNDANT="${1#*=}"
    #   ;;
    *)

      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

pushd "./${PROG_NAME}" || { echo "Cannot cd ${PROG_NAME}..."; exit 1; }

# Do compile if needed
if [ "x${FORCE_COMP}" != x ]; then
  rm -r bin
fi
if [[ ${METRICS} = "false" ]]; then
  export POSTFIX="_no_metrics"  
  make "ELEMS=${ELEMS}" "LENGTH=${LENGTH}" "MODE=${MODE}"\
    "COMPILER=${COMPILER}" METRIC_FLAG=NULL "THREADS=${EXP_THREADS}"
fi

if [[ ${METRICS} = "true" ]]; then
  export POSTFIX="_with_metrics"  
  make "ELEMS=${ELEMS}" "LENGTH=${LENGTH}" "MODE=${MODE}"\
    "COMPILER=${COMPILER}" METRIC_FLAG=METRIC_RUN "THREADS=${EXP_THREADS}"
fi

rm "${OUTPUT}"
# Run benchmark if needed
if [ "x${NO_RUN}" = "xfalse" ]; then
  PROG="./bin/omp_${PROG_NAME}_np_STD${POSTFIX}"
  export OMP_NUM_THREADS=${EXP_THREADS}
  export OMP_PROC_BIND=true
  export OMP_PROC_BIND=close
  export LD_LIBRARY_PATH=/opt/intel/oneapi/compiler/2021.1.2/linux/compiler/lib/intel64_lin/:${LD_LIBRARY_PATH}
  if [[ "x${METRICS}" = "xtrue" ]]; then
    perf stat -o "${OUTPUT}" -a -e "${EVENTS}" "${PROG}"
  fi
  if [[ ${METRICS} = "false" ]]; then
    ${PROG} > "${OUTPUT}"
    cp "${OUTPUT}" ./test.txt
  fi
fi

popd || echo "Whooops! Cannot return from ${PROG_NAME}..."


