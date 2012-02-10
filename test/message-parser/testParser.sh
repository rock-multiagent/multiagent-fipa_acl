#!/bin/sh
TESTBINARY=../../build/bin/testParser 

validate()
{
  FILENAME=$1
  ERROR=
  OUTPUT=`${TESTBINARY} -b $FILENAME`
  ERROR=`echo $OUTPUT | grep -i failed`

#  echo $OUTPUT

  if [ "$ERROR" != "" ]; then 
	echo "$FILENAME \t\t [ERROR]"
  else 
	echo "$FILENAME \t\t [OK]"
  fi
}

echo "Positive tests"
for file in `ls *.txt`
do 
    validate $file
done

for file in `ls positive-test-messages/*`
do 
    validate $file
done

echo ""
echo "Negative tests"
for file in `ls negative-test-messages/*`
do
    validate $file
done
