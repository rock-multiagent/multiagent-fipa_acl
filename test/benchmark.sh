#!/bin/sh
TIMESTAMP=`date +%Y%m%d_%H:%M:%S`
OUTPUT=results-$TIMESTAMP.txt
BENCHMARK=../build/bin/fipa_acl-benchmark
touch $OUTPUT

`${BENCHMARK} 0 >> ${OUTPUT}`

size=1
for exp in `seq 0 24`
do
   size=`echo 2^$exp | bc`
   echo "${BENCHMARK} ${size} >> ${OUTPUT}"
   `${BENCHMARK} ${size} >> ${OUTPUT}`
done
