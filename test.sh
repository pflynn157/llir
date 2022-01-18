#!/bin/bash
export TEST_DIR=./build/test_bin

echo "Running all tests..."
echo ""

(
cd $TEST_DIR
for d in ./*/
do
    CURRENT=`basename $d`
    echo $CURRENT
    ./$CURRENT
    if [ $? == "0" ] ; then
        echo "Success"
    else
        echo "Fail"
    fi
    echo ""
done
)

echo "Done!"
echo ""

