#!/bin/sh
TEST_DIR=`echo $0 | sed 's/\(.*\)\/.*/\1/'`

TESTBINARY="$TEST_DIR/../../build/src/testParser"

validate()
{
  FILENAME=$1
  ERROR=
  OUTPUT=`${TESTBINARY} -b $FILENAME`
  if [ $? -ne 0 ]; then
      ERROR="EXCEPTION"
  else
      ERROR=`echo $OUTPUT | egrep -i "failed|error"`
  fi

  if [ "$ERROR" != "" ]; then 
	echo "${FILENAME} [ERROR]"
  else 
	echo "${FILENAME}  [OK]"
  fi
}

echo "Positive tests"
for file in `ls $TEST_DIR/*.txt`
do 
    validate $file
done

for file in `ls $TEST_DIR/positive-test-messages/*`
do 
    validate $file
done

echo ""
echo "Negative tests -> should return ERROR"
for file in `ls $TEST_DIR/negative-test-messages/*`
do
    validate $file
done
